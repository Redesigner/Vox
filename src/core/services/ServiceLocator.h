#pragma once

#include <memory>

struct SDL_Window;

namespace Vox
{
	class InputService;
	class Renderer;
	class PhysicsServer;

	class ServiceLocator
	{
	public:
		static void InitServices(SDL_Window* window);
		static void DeleteServices();

		static InputService* GetInputService();

		static Renderer* GetRenderer();

		static PhysicsServer* GetPhysicsServer();

	private:
		static InputService* inputService;
		static Renderer* renderer;
		static PhysicsServer* physicsServer;
	};
}