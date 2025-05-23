#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <vector>
#include <unordered_map>

#include "core/config/Config.h"

#include <glm/vec2.hpp>
#include <SDL3/SDL_events.h>

#include "core/datatypes/Delegate.h"

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
    using MouseReleaseEventCallback = std::function<void()>;

	class InputService
	{
	public:
		InputService(SDL_Window* window);
		~InputService();

		void PollEvents();


		const KeyboardEventCallback& RegisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback);
		void UnregisterKeyboardCallback(SDL_Scancode scancode, const KeyboardEventCallback& callback);


		const MouseMotionEventCallback& RegisterMouseMotionCallback(MouseMotionEventCallback callback);
		void UnregisterMouseMotionCallback(const MouseMotionEventCallback& callback);


		const DelegateHandle<void(int, int)> RegisterMouseClickCallback(MouseClickEventCallback callback);
		void UnregisterMouseClickCallback(const DelegateHandle<int, int>& handle);

	    const MouseReleaseEventCallback& RegisterMouseReleaseCallback(MouseReleaseEventCallback callback);
		void UnregisterMouseReleaseCallback(const MouseReleaseEventCallback& callback);


		[[nodiscard]] glm::vec2 GetInputAxisNormalized(KeyboardInputAxis2D input) const;

	    [[nodiscard]] glm::vec2 GetMousePosition() const;

		[[nodiscard]] bool ShouldCloseWindow() const;

		void ToggleCursorLock();

		[[nodiscard]] bool IsWindowFullscreen() const;

		[[nodiscard]] bool IsWindowMaximized() const;

	private:
		void HandleEvent(SDL_Event* event);

		void HandleWindowEvent(SDL_WindowEvent& windowEvent);

		void HandleMouseMotionEvent(SDL_MouseMotionEvent& mouseEvent);

		void HandleMouseButtonEvent(SDL_MouseButtonEvent& mouseEvent);

	    void HandleMouseButtonReleaseEvent(SDL_MouseButtonEvent& mouseEvent);

		void ExecuteCallbacks(SDL_Scancode scancode, bool pressed);

		void ToggleFullscreen();

		std::unordered_map<SDL_Scancode, std::vector<KeyboardEventCallback>> keyboardEventMap;
		std::vector<MouseMotionEventCallback> mouseMotionEventCallbacks;
		Delegate<int, int> mouseClickEventCallbacks;
	    std::vector<MouseReleaseEventCallback> mouseReleaseEventCallbacks;

		// @TODO: move window logic and variables to a separate wrapper class?
		bool windowClosed = false;

		bool cursorLocked = false;

		bool keyPressed[SDL_SCANCODE_COUNT]{ false };

		bool windowFullscreen;

		bool windowMaximized;

		SDL_Window* mainWindow;
	};
}