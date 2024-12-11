#include "PhysicsServer.h"

#include <thread>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include "raylib.h"
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
		debugRenderer = std::make_unique<Vox::DebugRenderer>();

		const JPH::uint cMaxBodies = 1024;
		const JPH::uint cNumBodyMutexes = 0;
		const JPH::uint cMaxBodyPairs = 1024;
		const JPH::uint cMaxContactConstraints = 1024;

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
		for (CharacterController& characterController : characterControllers)
		{
			characterController.Update(fixedTimeStep, this);
		}
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
			TraceLog(LOG_ERROR, "[Physics] Failed to get character controller velocity. Character controller id was invalid.");
			return JPH::Vec3::sZero();
		}

		return characterControllers[id].GetVelocity();
	}

	JPH::Vec3 PhysicsServer::GetCharacterControllerPosition(CharacterControllerId id) const
	{
		if (id >= characterControllers.size())
		{
			TraceLog(LOG_ERROR, "[Physics] Failed to get character controller position. Character controller id was invalid.");
			return JPH::Vec3::sZero();
		}

		return characterControllers[id].GetPosition();
	}

	JPH::Vec3 PhysicsServer::GetObjectPosition(const JPH::BodyID& id) const
	{
		const JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
		if (!bodyInterface.IsAdded(id))
		{
			TraceLog(LOG_WARNING, TextFormat("[PhysicsServer] Unable to get object position. The body ID was not added to the body interface."));
			return JPH::Vec3(0.0f, 0.0f, 0.0f);
		}

		return bodyInterface.GetPosition(id);
	}

	void PhysicsServer::RenderDebugShapes()
	{
		JPH::BodyManager::DrawSettings drawSettings = JPH::BodyManager::DrawSettings();
		physicsSystem.DrawBodies(drawSettings, debugRenderer.get());
		physicsSystem.DrawConstraints(debugRenderer.get());
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
}