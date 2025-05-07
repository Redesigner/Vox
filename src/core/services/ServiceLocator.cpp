#include "ServiceLocator.h"

#include "FileIOService.h"
#include "ObjectService.h"
#include "core/services/InputService.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	FileIOService* ServiceLocator::fileIoService = nullptr;
	InputService* ServiceLocator::inputService = nullptr;
	ObjectService* ServiceLocator::objectService = nullptr;
	PhysicsServer* ServiceLocator::physicsServer = nullptr;
	Renderer* ServiceLocator::renderer = nullptr;

	void ServiceLocator::InitServices(SDL_Window* window)
	{
		fileIoService = new FileIOService();
		inputService = new InputService(window);
		objectService = new ObjectService();
		physicsServer = new PhysicsServer();
		renderer = new Renderer(window);
	}

	void ServiceLocator::DeleteServices()
	{
		delete fileIoService;
		delete inputService;
		delete objectService;
		delete physicsServer;
		delete renderer;
	}

	FileIOService* ServiceLocator::GetFileIoService()
	{
		return fileIoService;
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
