#include "ServiceLocator.h"

#include "core/services/InputService.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	InputService* ServiceLocator::inputService = nullptr;
	Renderer* ServiceLocator::renderer = nullptr;
	PhysicsServer* ServiceLocator::physicsServer = nullptr;

	void ServiceLocator::InitServices(SDL_Window* window)
	{
		inputService = new InputService(window);
		renderer = new Renderer(window);
		physicsServer = new PhysicsServer();
	}

	void ServiceLocator::DeleteServices()
	{
		delete inputService;
		delete renderer;
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
}