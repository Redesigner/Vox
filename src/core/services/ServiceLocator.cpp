#include "ServiceLocator.h"

#include "ObjectService.h"
#include "core/services/InputService.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	InputService* ServiceLocator::inputService = nullptr;
	Renderer* ServiceLocator::renderer = nullptr;
	PhysicsServer* ServiceLocator::physicsServer = nullptr;
	ObjectService* ServiceLocator::objectService = nullptr;

	void ServiceLocator::InitServices(SDL_Window* window)
	{
		inputService = new InputService(window);
		renderer = new Renderer(window);
		physicsServer = new PhysicsServer();
		objectService = new ObjectService();
	}

	void ServiceLocator::DeleteServices()
	{
		delete inputService;
		delete renderer;
		delete physicsServer;
		delete objectService;
	}

	InputService* ServiceLocator::GetInputService()
	{
		return inputService;
	}

	Renderer* ServiceLocator::GetRenderer()
	{
		return renderer;
	}

	PhysicsServer* ServiceLocator::GetPhysicsServer()
	{
		return physicsServer;
	}

	ObjectService* ServiceLocator::GetObjectService()
	{
		return objectService;
	}
}
