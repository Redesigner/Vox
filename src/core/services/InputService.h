#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <vector>
#include <unordered_map>

#include "core/config/Config.h"

#include <glm/vec2.hpp>
#include <SDL3/SDL_events.h>

struct SDL_Window;

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
		InputService(SDL_Window* window);
		~InputService();

		void PollEvents();


		void RegisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback);

		void UnregisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback);


		void RegisterMouseMotionCallback(MouseMotionEventCallback callback);

		void UnregisterMouseMotionCallback(MouseMotionEventCallback callback);


		void RegisterMouseClickCallback(const MouseClickEventCallback& callback);

		void UnregisterMouseClickCallback(MouseClickEventCallback callback);


		glm::vec2 GetInputAxisNormalized(KeyboardInputAxis2D input) const;

		bool ShouldCloseWindow() const;

		void ToggleCursorLock();

		bool IsWindowFullscreen() const;

		bool IsWindowMaximized() const;

	private:
		void HandleEvent(SDL_Event* event);

		void HandleWindowEvent(SDL_WindowEvent& windowEvent);

		void HandleMouseMotionEvent(SDL_MouseMotionEvent& mouseEvent);

		void HandleMouseButtonEvent(SDL_MouseButtonEvent& mouseEvent);

		void ExecuteCallbacks(SDL_Scancode scancode, bool pressed);

		void ToggleFullscreen();

		std::unordered_map<SDL_Scancode, std::vector<KeyboardEventCallback>> keyboardEventMap;
		std::vector<MouseMotionEventCallback> mouseMotionEventCallbacks;
		std::vector<MouseClickEventCallback> mouseClickEventCallbacks;

		// @TODO: move window logic and variables to a separate wrapper class?
		bool windowClosed = false;

		bool cursorLocked = false;

		bool keyPressed[SDL_SCANCODE_COUNT]{ false };

		bool windowFullscreen;

		bool windowMaximized;

		SDL_Window* mainWindow;
	};
}