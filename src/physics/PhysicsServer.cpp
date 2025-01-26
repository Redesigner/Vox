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

#include "core/logging/Logging.h"
#include "rendering/DebugRenderer.h"

namespace Vox
{
	PhysicsServer::PhysicsServer()
	{
		JPH::RegisterDefaultAllocator();

		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();

		const JPH::uint cMaxPhysicsJobs = 64;
		const JPH::uint cMaxPhysicsBarriers = 8;

		jobSystem = std::make_unique<JPH::JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
		tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

		const JPH::uint cMaxBodies = 1024;
		const JPH::uint cNumBodyMutexes = 0;
		const JPH::uint cMaxBodyPairs = 1024;
		const JPH::uint cMaxContactConstraints = 1024;

		voxelBodies = ObjectContainer<VoxelBody>(8);

		physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
			broadPhaseLayerImplementation, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);

		physicsSystem.SetContactListener(&contactListener);
		JPH::DebugRenderer::sInstance = debugRenderer.get();
	}

	PhysicsServer::~PhysicsServer()
	{
		JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		for (const JPH::BodyID bodyId : bodyIds)
		{
			bodyInterface.RemoveBody(bodyId);
			bodyInterface.DestroyBody(bodyId);
		}

		JPH::UnregisterTypes();

		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	void PhysicsServer::Step()
	{
		++stepCount;
		StepCharacterControllers();
		UpdateSpringArms();
		UpdateVoxelBodies();
		physicsSystem.Update(fixedTimeStep, 1, tempAllocator.get(), jobSystem.get());
	}

	JPH::BodyID PhysicsServer::CreateStaticBox(JPH::RVec3 size, JPH::Vec3 position)
	{
		JPH::BoxShapeSettings boxShapeSettings = JPH::BoxShapeSettings(size / 2.0f);
		return CreateStaticShape(boxShapeSettings.Create().Get(), position);
	}

	JPH::BodyID PhysicsServer::CreatePlayerCapsule(float radius, float halfHeight, JPH::Vec3 position)
	{
		JPH::CapsuleShapeSettings capsuleShapeSettings = JPH::CapsuleShapeSettings(halfHeight, radius);
		JPH::BodyID capsuleId = CreateDynamicShape(capsuleShapeSettings.Create().Get(), position);
		// physicsSystem.GetBodyInterface().SetLinearVelocity(capsuleId, JPH::Vec3(0.0f, -0.01f, 0.0f));
		return capsuleId;
	}

	unsigned int PhysicsServer::CreateCharacterController(float radius, float halfHeight)
	{
		characterControllers.emplace_back(radius, halfHeight, &physicsSystem);
		return characterControllers.size() - 1;
	}

	JPH::Vec3 PhysicsServer::GetCharacterControllerVelocity(CharacterControllerId id) const
	{
		if (id >= characterControllers.size())
		{
			VoxLog(Error, Physics, "Failed to get character controller velocity. Character controller id was invalid.");
			return JPH::Vec3::sZero();
		}

		return characterControllers[id].GetVelocity();
	}

	JPH::Vec3 PhysicsServer::GetCharacterControllerPosition(CharacterControllerId id) const
	{
		if (id >= characterControllers.size())
		{
			VoxLog(Error, Physics, "Failed to get character controller position.Character controller id was invalid.");
			return JPH::Vec3::sZero();
		}

		return characterControllers[id].GetPosition();
	}

	JPH::Quat PhysicsServer::GetCharacterControllerRotation(CharacterControllerId id) const
	{
		if (id >= characterControllers.size())
		{
			VoxLog(Error, Physics, "Failed to get character controller rotation. Character controller id was invalid.");
			return JPH::Quat::sIdentity();
		}

		return characterControllers[id].GetRotation();
	}

	void PhysicsServer::SetCharacterControllerYaw(CharacterControllerId id, float yaw)
	{
		if (id >= characterControllers.size())
		{
			VoxLog(Error, Physics, "Failed to set character controller yaw. Character controller id was invalid.");
			return;
		}

		characterControllers[id].SetYaw(yaw);
	}

	unsigned int PhysicsServer::CreateSpringArm(CharacterControllerId id)
	{
		SpringArm& newArm = springArms.emplace_back();
		unsigned int resultId = springArms.size() - 1;
		newArm.SetOrigin(id);
		newArm.SetLength(4.0f);
		newArm.SetOffset({ 0.0f, 1.0f, 0.0f });
		return resultId;
	}

	unsigned int PhysicsServer::CreateSpringArm(JPH::BodyID bodyId)
	{
		SpringArm& newArm = springArms.emplace_back();
		unsigned int resultId = springArms.size() - 1;
		newArm.SetOrigin(bodyId);
		return resultId;
	}

	void PhysicsServer::SetSpringArmEulerRotation(SpringArmId id, JPH::Vec3 rotation)
	{
		if (SpringArm* springArm = GetSpringArm(id))
		{
			springArm->SetEulerRotation(rotation);
		}
	}

	JPH::Vec3 PhysicsServer::GetSpringArmEulerRotation(SpringArmId id) const
	{
		if (const SpringArm* springArm = GetSpringArm(id))
		{
			return springArm->GetEulerRotation();
		}

		return JPH::Vec3::sZero();
	}

	JPH::Vec3 PhysicsServer::GetSpringArmResult(SpringArmId id) const
	{
		if (const SpringArm* springArm = GetSpringArm(id))
		{
			return springArm->GetResultPosition();
		}

		return JPH::Vec3::sZero();
	}

	JPH::Vec3 PhysicsServer::GetSpringArmOrigin(SpringArmId id) const
	{
		if (const SpringArm* springArm = GetSpringArm(id))
		{
			return springArm->GetOrigin();
		}

		return JPH::Vec3::sZero();
	}

	JPH::Vec3 PhysicsServer::GetObjectPosition(const JPH::BodyID& id) const
	{
		const JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		if (!bodyInterface.IsAdded(id))
		{
			VoxLog(Warning, Physics, "Unable to get object position. The body ID was not added to the body interface.");
			return JPH::Vec3(0.0f, 0.0f, 0.0f);
		}

		return bodyInterface.GetPosition(id);
	}

	void PhysicsServer::RenderDebugShapes()
	{
		JPH::BodyManager::DrawSettings drawSettings = JPH::BodyManager::DrawSettings();
		// physicsSystem.DrawBodies(drawSettings, debugRenderer.get());
		physicsSystem.DrawConstraints(debugRenderer.get());
		for (CharacterController& characterController : characterControllers)
		{
			debugRenderer->DrawCapsule(JPH::Mat44::sTranslation(characterController.GetPosition()), characterController.GetHalfHeight(), characterController.GetRadius(), JPH::ColorArg::sRed);
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

	void PhysicsServer::SetDebugRenderer(std::shared_ptr<DebugRenderer> debugRenderer)
	{
		this->debugRenderer = debugRenderer;
	}

	void PhysicsServer::StepCharacterControllers()
	{
		for (CharacterController& characterController : characterControllers)
		{
			characterController.Update(fixedTimeStep, this);
		}
	}

	void PhysicsServer::UpdateSpringArms()
	{
		for (SpringArm& springArm : springArms)
		{
			springArm.Update(this);
		}
	}

	void PhysicsServer::UpdateVoxelBodies()
	{
		JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		for (const std::pair<size_t, int>& index : voxelBodies.GetDiryIndices())
		{
			if (VoxelBody* body = voxelBodies.Get(index.first, index.second))
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

	JPH::BodyID PhysicsServer::CreateStaticShape(JPH::Shape* shape, const JPH::Vec3& position)
	{
		JPH::BodyCreationSettings bodyCreationSettings(shape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Static, Physics::CollisionLayer::Static);
		JPH::BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, JPH::EActivation::Activate);
		bodyIds.push_back(bodyId);
		return bodyId;
	}

	JPH::BodyID PhysicsServer::CreateDynamicShape(JPH::Shape* shape, const JPH::Vec3& position)
	{
		JPH::BodyCreationSettings bodyCreationSettings(shape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Physics::CollisionLayer::Dynamic);
		JPH::BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, JPH::EActivation::Activate);
		bodyIds.push_back(bodyId);
		return bodyId;
	}

	JPH::BodyID PhysicsServer::CreateCompoundShape(JPH::StaticCompoundShapeSettings* settings)
	{
		using namespace JPH;
		JPH::BodyCreationSettings bodyCreationSettings(settings, Vec3::sZero(), Quat::sIdentity(), EMotionType::Static, Physics::CollisionLayer::Static);
		BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, EActivation::Activate);
		// physicsSystem.
		return bodyId;
	}

	Ref<VoxelBody> PhysicsServer::CreateVoxelBody()
	{
		return Ref<VoxelBody>(&voxelBodies, voxelBodies.Create(32));
	}

	bool PhysicsServer::RayCast(JPH::Vec3 origin, JPH::Vec3 direction, RayCastResultNormal& resultOut)
	{
		using namespace JPH;
		RRayCast rayCast = RRayCast(origin, direction);
		RayCastResult rayCastResult = RayCastResult();
		resultOut.origin = origin;
		if (physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, rayCastResult))
		{
			resultOut.hitBody = rayCastResult.mBodyID;
			resultOut.impactPoint = origin + direction * rayCastResult.mFraction;

			BodyLockRead lock = BodyLockRead(physicsSystem.GetBodyLockInterfaceNoLock(), rayCastResult.mBodyID);
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

	const SpringArm* PhysicsServer::GetSpringArm(SpringArmId id) const
	{
		if (id >= springArms.size())
		{
			VoxLog(Warning, Physics, "Unable to get spring arm. The spring arm ID was not valid.");
			return nullptr;
		}

		return &springArms[id];
	}

	SpringArm* PhysicsServer::GetSpringArm(SpringArmId id)
	{
		if (id >= springArms.size())
		{
			VoxLog(Warning, Physics, "Unable to get spring arm. The spring arm ID was not valid.");
			return nullptr;
		}

		return &springArms[id];
	}
}