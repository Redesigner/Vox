#pragma once

#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "raylib.h"

#include <SDL2/SDL_events.h>

namespace Vox
{
	struct KeyboardInputAxis2D
	{
		SDL_Scancode yPos, yNeg, xPos, xNeg;
		KeyboardInputAxis2D(SDL_Scancode yPos, SDL_Scancode yNeg, SDL_Scancode xPos, SDL_Scancode xNeg);
	};

	using KeyboardEventCallback = std::function<void(bool)>;
	using MouseMotionEventCallback = std::function<void(int, int)>;
	using MouseClickEventCallback = std::function<void(int, int)>;

	class InputService
	{
	public:
		InputService();
		~InputService();

		void PollEvents();


		void RegisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback);

		void UnregisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback);


		void RegisterMouseMotionCallback(MouseMotionEventCallback callback);

		void UnregisterMouseMotionCallback(MouseMotionEventCallback callback);


		void RegisterMouseClickCallback(MouseClickEventCallback callback);

		void UnregisterMouseClickCallback(MouseClickEventCallback callback);


		Vector2 GetInputAxisNormalized(KeyboardInputAxis2D input) const;

		bool ShouldCloseWindow() const;

		void ToggleCursorLock();

	private:
		void HandleEvent(SDL_Event* event);

		void HandleWindowEvent(SDL_WindowEvent& windowEvent);

		void HandleMouseMotionEvent(SDL_MouseMotionEvent& mouseEvent);

		void HandleMouseButtonEvent(SDL_MouseButtonEvent& mouseEvent);

		void ExecuteCallbacks(SDL_Scancode scancode, bool pressed);

		std::unordered_map<SDL_Scancode, std::vector<KeyboardEventCallback>> keyboardEventMap;
		std::vector<MouseMotionEventCallback> mouseMotionEventCallbacks;
		std::vector<MouseClickEventCallback> mouseClickEventCallbacks;

		bool windowClosed = false;

		bool cursorLocked = false;

		bool keyPressed[SDL_NUM_SCANCODES]{ false };
	};
}