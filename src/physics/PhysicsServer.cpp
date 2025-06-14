#include "PhysicsServer.h"

#include <thread>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include "TypeConversions.h"
#include "core/logging/Logging.h"
#include "rendering/DebugRenderer.h"

namespace Vox
{
	PhysicsServer::PhysicsServer()
	{
	    if (!JPH::Factory::sInstance)
	    {
	        JPH::RegisterDefaultAllocator();
	        JPH::Factory::sInstance = new JPH::Factory();
	        JPH::RegisterTypes();
	    }

        constexpr JPH::uint cMaxPhysicsJobs = 64;
        constexpr JPH::uint cMaxPhysicsBarriers = 8;

		jobSystem = std::make_unique<JPH::JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
		tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

        constexpr JPH::uint cMaxBodies = 1024;
        constexpr JPH::uint cNumBodyMutexes = 0;
        constexpr JPH::uint cMaxBodyPairs = 1024;
        constexpr JPH::uint cMaxContactConstraints = 1024;

		voxelBodies = DynamicObjectContainer<VoxelBody>(8);

		physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
			broadPhaseLayerImplementation, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);

		physicsSystem.SetContactListener(&contactListener);
	}

	PhysicsServer::~PhysicsServer()
	{
		JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		for (const JPH::BodyID bodyId : bodyIds)
		{
			bodyInterface.RemoveBody(bodyId);
			bodyInterface.DestroyBody(bodyId);
		}
	}

	void PhysicsServer::Step()
	{
	    if (!running)
	    {
	        return;
	    }

		++stepCount;
		StepCharacterControllers(fixedTimeStep);
		UpdateVoxelBodies();
		physicsSystem.Update(fixedTimeStep, 1, tempAllocator.get(), jobSystem.get());
	}

	JPH::BodyID PhysicsServer::CreateStaticBox(const JPH::RVec3 size, const JPH::Vec3 position)
	{
		const auto boxShapeSettings = JPH::BoxShapeSettings(size / 2.0f);
		return CreateStaticShape(boxShapeSettings.Create().Get(), position);
	}

	JPH::BodyID PhysicsServer::CreatePlayerCapsule(const float radius, const float halfHeight, const JPH::Vec3 position)
	{
		const auto capsuleShapeSettings = JPH::CapsuleShapeSettings(halfHeight, radius);
		const JPH::BodyID capsuleId = CreateDynamicShape(capsuleShapeSettings.Create().Get(), position);
		// physicsSystem.GetBodyInterface().SetLinearVelocity(capsuleId, JPH::Vec3(0.0f, -0.01f, 0.0f));
		return capsuleId;
	}

	std::shared_ptr<CharacterController> PhysicsServer::CreateCharacterController(float radius, float halfHeight)
	{
	    auto result = std::make_shared<CharacterController>(radius, halfHeight, &physicsSystem);
		characterControllers.emplace_back(result);
		return result;
	}

	JPH::Vec3 PhysicsServer::GetObjectPosition(const JPH::BodyID& id) const
	{
		const JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		if (!bodyInterface.IsAdded(id))
		{
			VoxLog(Warning, Physics, "Unable to get object position. The body ID was not added to the body interface.");
			return {0.0f, 0.0f, 0.0f};
		}

		return bodyInterface.GetPosition(id);
	}

	void PhysicsServer::RenderDebugShapes()
	{
		physicsSystem.DrawConstraints(debugRenderer.get());

		for (const auto& characterControllerWeak : characterControllers)
		{
			const auto& characterController = characterControllerWeak.lock();
		    if (!characterController)
		    {
		        continue;
		    }

			debugRenderer->DrawCapsule(JPH::Mat44::sTranslation(characterController->GetPosition()), characterController->GetHalfHeight(), characterController->GetRadius(), JPH::ColorArg::sRed);
		}
	}

	Vox::DebugRenderer* PhysicsServer::GetDebugRenderer() const
	{
		return debugRenderer.get();
	}

	JPH::PhysicsSystem* PhysicsServer::GetPhysicsSystem()
	{
		return &physicsSystem;
	}

	JPH::TempAllocator* PhysicsServer::GetAllocator() const
	{
		return tempAllocator.get();
	}

	void PhysicsServer::SetDebugRenderer(const std::shared_ptr<DebugRenderer>& debugRenderer)
	{
		this->debugRenderer = debugRenderer;
        JPH::DebugRenderer::sInstance = debugRenderer.get();
	}


    // ReSharper disable once CppDFAConstantParameter
    // For now anyways, this will be constant
    void PhysicsServer::StepCharacterControllers(const float deltaTime)
	{
		for (auto& characterControllerWeak : characterControllers)
		{
            if (const auto characterController = characterControllerWeak.lock())
			{
				characterController->Update(deltaTime, this);
			}
		}

	    // Clean up expired pointers
	    std::erase_if(characterControllers, [](const std::weak_ptr<CharacterController>& characterController){ return characterController.expired(); });
	}

	void PhysicsServer::UpdateVoxelBodies()
	{
		JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		for (const auto& [id, index] : voxelBodies.GetDirtyIndices())
		{
			if (VoxelBody* body = voxelBodies.Get(id, index))
			{
				if (!body->GetBodyId().IsInvalid())
				{
					VoxLog(Display, Physics, "Destroying voxel body.");
					bodyInterface.RemoveBody(body->GetBodyId());
					bodyInterface.DestroyBody(body->GetBodyId());
				}
				VoxLog(Display, Physics, "Creating new voxel body.");
				body->SetBodyId(CreateCompoundShape(body->GetShapeSettings()));
			}
		}
		voxelBodies.ClearDirty();
	}

	JPH::BodyID PhysicsServer::CreateStaticShape(const JPH::Shape* shape, const JPH::Vec3& position)
	{
		const JPH::BodyCreationSettings bodyCreationSettings(shape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Static, Physics::CollisionLayer::Static);
		const JPH::BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, JPH::EActivation::Activate);
		bodyIds.push_back(bodyId);
		return bodyId;
	}

	JPH::BodyID PhysicsServer::CreateDynamicShape(const JPH::Shape* shape, const JPH::Vec3& position)
	{
		const JPH::BodyCreationSettings bodyCreationSettings(shape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Physics::CollisionLayer::Dynamic);
		const JPH::BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, JPH::EActivation::Activate);
		bodyIds.push_back(bodyId);
		return bodyId;
	}

	JPH::BodyID PhysicsServer::CreateCompoundShape(const JPH::StaticCompoundShapeSettings* settings)
	{
		using namespace JPH;
		const BodyCreationSettings bodyCreationSettings(settings, Vec3::sZero(), Quat::sIdentity(), EMotionType::Static, Physics::CollisionLayer::Static);
		const BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, EActivation::Activate);
		return bodyId;
	}

	DynamicRef<VoxelBody> PhysicsServer::CreateVoxelBody()
	{
		return DynamicRef<VoxelBody>(&voxelBodies, voxelBodies.Create());
	}

	bool PhysicsServer::RayCast(const JPH::Vec3 origin, const JPH::Vec3 direction, RayCastResultNormal& resultOut) const
    {
		using namespace JPH;
		RRayCast rayCast = RRayCast(origin, direction);
		RayCastResult rayCastResult = RayCastResult();
		resultOut.origin = origin;
		if (physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, rayCastResult))
		{
			resultOut.hitBody = rayCastResult.mBodyID;
			resultOut.impactPoint = origin + direction * rayCastResult.mFraction;\
		    resultOut.percentage = rayCastResult.mFraction;

			const auto lock = BodyLockRead(physicsSystem.GetBodyLockInterfaceNoLock(), rayCastResult.mBodyID);
			if (lock.Succeeded())
			{
				const Body& hitBody = lock.GetBody();
				resultOut.impactNormal = hitBody.GetWorldSpaceSurfaceNormal(rayCastResult.mSubShapeID2, resultOut.impactPoint);
			}
			else
			{
				VoxLog(Warning, Physics, "Failed to return raycast result. The ray hit a body, but the body was locked. Make sure you aren't trying to read while the physics is updating.");
			}
			return true;
		}
		return false;
	}

    bool PhysicsServer::RayCast(const glm::vec3 origin, const glm::vec3 direction, RayCastResultNormal& resultOut) const
    {
        return RayCast(Vec3From(origin), Vec3From(direction), resultOut);
    }
}
