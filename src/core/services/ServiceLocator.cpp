#include "ServiceLocator.h"

#include "EditorService.h"
#include "FileIOService.h"
#include "ObjectService.h"
#include "core/services/InputService.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	EditorService* ServiceLocator::editorService = nullptr;
	FileIOService* ServiceLocator::fileIoService = nullptr;
	InputService* ServiceLocator::inputService = nullptr;
	ObjectService* ServiceLocator::objectService = nullptr;
	Renderer* ServiceLocator::renderer = nullptr;

	void ServiceLocator::InitServices(SDL_Window* window)
	{
		fileIoService = new FileIOService();
		inputService = new InputService(window);
		objectService = new ObjectService();
		renderer = new Renderer(window);
		editorService = new EditorService();
	}

	void ServiceLocator::DeleteServices()
	{
		delete objectService;
		delete editorService;
		delete fileIoService;
		delete inputService;
		delete renderer;
	    inputService = nullptr;
	    editorService = nullptr;
	    fileIoService = nullptr;
	    objectService = nullptr;
	    renderer = nullptr;
	}

	EditorService* ServiceLocator::GetEditorService()
	{
		return editorService;
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

	ObjectService* ServiceLocator::GetObjectService()
	{
		return objectService;
	}
}
