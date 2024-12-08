#pragma once

#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>

#include <SDL2/SDL_events.h>

namespace Vox
{
	using KeyboardEventCallback = std::function<void(bool)>;

	class InputService
	{
	public:
		InputService();
		~InputService();

		void PollEvents();

		void RegisterCallback(SDL_Scancode scancode, KeyboardEventCallback callback);

		void UnregisterCallback(SDL_Scancode scancode, KeyboardEventCallback callback);

		bool ShouldCloseWindow() const;

	private:
		void HandleEvent(SDL_Event* event);

		void HandleWindowEvent(SDL_WindowEvent& windowEvent);

		void ExecuteCallbacks(SDL_Scancode scancode, bool pressed);

		std::unordered_map<SDL_Scancode, std::vector<KeyboardEventCallback>> keyboardEventMap;

		bool windowClosed = false;

		bool keyPressed[SDL_NUM_SCANCODES]{ false };
	};
}