#pragma once

#include <memory>

namespace Vox
{
	class InputService;

	class ServiceLocator
	{
	public:
		static void InitServices();
		static void DeleteServices();

		static InputService* GetInputService();

	private:
		static InputService* inputService;
	};
}