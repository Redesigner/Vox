#include "ServiceLocator.h"

#include "core/services/InputService.h"

Vox::InputService* Vox::ServiceLocator::inputService = nullptr;

void Vox::ServiceLocator::InitServices(SDL_Window* window)
{
	inputService = new Vox::InputService(window);
}

void Vox::ServiceLocator::DeleteServices()
{
	delete inputService;
}

Vox::InputService* Vox::ServiceLocator::GetInputService()
{
	return inputService;
}
