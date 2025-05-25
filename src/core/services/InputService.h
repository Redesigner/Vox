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
    struct KeyboardInputAxis
    {
        SDL_Scancode positive, negative;
        KeyboardInputAxis(SDL_Scancode positive, SDL_Scancode negative);
    };

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


		[[nodiscard]] DelegateHandle<bool> RegisterKeyboardCallback(SDL_Scancode scancode, const KeyboardEventCallback& callback);
		void UnregisterKeyboardCallback(SDL_Scancode scancode, const DelegateHandle<bool>& callback);


		[[nodiscard]] DelegateHandle<int, int> RegisterMouseMotionCallback(const MouseMotionEventCallback& callback);
		void UnregisterMouseMotionCallback(const DelegateHandle<int, int>& callback);


		[[nodiscard]] DelegateHandle<int, int> RegisterMouseClickCallback(const MouseClickEventCallback& callback);
		void UnregisterMouseClickCallback(const DelegateHandle<int, int>& handle);

	    [[nodiscard]] DelegateHandle<> RegisterMouseReleaseCallback(MouseReleaseEventCallback callback);
		void UnregisterMouseReleaseCallback(const DelegateHandle<>& callback);

	    [[nodiscard]] DelegateHandle<int, int> RegisterMouseRightClickCallback(const MouseClickEventCallback& callback);
		void UnregisterMouseRightClickCallback(const DelegateHandle<int, int>& handle);

	    [[nodiscard]] DelegateHandle<> RegisterMouseRightReleaseCallback(const MouseReleaseEventCallback& callback);
		void UnregisterMouseRightReleaseCallback(const DelegateHandle<>& callback);


		[[nodiscard]] glm::vec2 GetInputAxisNormalized(KeyboardInputAxis2D input) const;

	    [[nodiscard]] float GetInputAxis(KeyboardInputAxis input) const;

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

		std::unordered_map<SDL_Scancode, Delegate<bool>> keyboardEventMap;
		Delegate<int, int> mouseMotionEventCallbacks;

		Delegate<int, int> mouseClickEventCallbacks;
	    Delegate<> mouseReleaseEventCallbacks;

        Delegate<int, int> mouseRightClickEventCallbacks;
	    Delegate<> mouseRightReleaseEventCallbacks;

		// @TODO: move window logic and variables to a separate wrapper class?
		bool windowClosed = false;

		bool cursorLocked = false;

		bool keyPressed[SDL_SCANCODE_COUNT]{ false };

		bool windowFullscreen;

		bool windowMaximized;

	    DelegateHandle<bool> escDelegate, fullscreenDelegate, tabDelegate;

		SDL_Window* mainWindow;
	};
}