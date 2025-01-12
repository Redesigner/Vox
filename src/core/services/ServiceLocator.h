#pragma once

#include <memory>

struct SDL_Window;

namespace Vox
{
	class InputService;

	class ServiceLocator
	{
	public:
		static void InitServices(SDL_Window* window);
		static void DeleteServices();

		static InputService* GetInputService();

	private:
		static InputService* inputService;
	};
}