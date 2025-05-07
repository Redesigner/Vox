#pragma once

struct SDL_Window;

namespace Vox
{
	class FileIOService;
	class InputService;
	class ObjectService;
	class PhysicsServer;
	class Renderer;

	class ServiceLocator
	{
	public:
		static void InitServices(SDL_Window* window);
		static void DeleteServices();

		static FileIOService* GetFileIoService();
		static InputService* GetInputService();
		static ObjectService* GetObjectService();
		static PhysicsServer* GetPhysicsServer();
		static Renderer* GetRenderer();

	private:
		static FileIOService* fileIoService;
		static InputService* inputService;
		static ObjectService* objectService;
		static PhysicsServer* physicsServer;
		static Renderer* renderer;
	};
}