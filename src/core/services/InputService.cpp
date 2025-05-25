#include "InputService.h"

#include <imgui_impl_sdl3.h>
#include <GL/glew.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

#include "core/logging/Logging.h"

namespace Vox
{
    InputService::InputService(SDL_Window* window)
        :mainWindow(window)
    {
        ToggleCursorLock();
        windowClosed = false;

        windowMaximized = false;
        windowFullscreen = false;

        // Register some default callbacks!
        escDelegate = RegisterKeyboardCallback(SDL_SCANCODE_ESCAPE, [this](bool pressed) { windowClosed = true; });
        tabDelegate = RegisterKeyboardCallback(SDL_SCANCODE_TAB, [this](bool pressed) { if (pressed) ToggleCursorLock(); });
        fullscreenDelegate = RegisterKeyboardCallback(SDL_SCANCODE_F11, [this](bool pressed) { if (pressed) ToggleFullscreen(); });
    }

    InputService::~InputService()
    {
    }

    void InputService::PollEvents()
    {
        SDL_Event polledEvent;
        while (SDL_PollEvent(&polledEvent))
        {
            HandleEvent(&polledEvent);
            ImGui_ImplSDL3_ProcessEvent(&polledEvent);

        }
    }

    DelegateHandle<bool> InputService::RegisterKeyboardCallback(SDL_Scancode scancode, const KeyboardEventCallback& callback)
    {
        const auto callbacks = keyboardEventMap.find(scancode);
        if (callbacks == keyboardEventMap.end())
        {
            auto [newIterator, success] = keyboardEventMap.emplace(scancode, std::move(Delegate<bool>()));
            return newIterator->second.RegisterCallback(callback);
        }

        return callbacks->second.RegisterCallback(callback);
    }

    void InputService::UnregisterKeyboardCallback(SDL_Scancode scancode, const DelegateHandle<bool>& callback)
    {
        auto callbacks = keyboardEventMap.find(scancode);
        if (callbacks == keyboardEventMap.end())
        {
            return;
        }

        callbacks->second.UnregisterCallback(callback);
    }

    DelegateHandle<int, int> InputService::RegisterMouseMotionCallback(const MouseMotionEventCallback& callback)
    {
        return mouseMotionEventCallbacks.RegisterCallback(callback);
    }

    void InputService::UnregisterMouseMotionCallback(const DelegateHandle<int, int>& callback)
    {
        mouseMotionEventCallbacks.UnregisterCallback(callback);
    }

    DelegateHandle<int, int> InputService::RegisterMouseClickCallback(const MouseClickEventCallback& callback)
    {
        return mouseClickEventCallbacks.RegisterCallback(callback);
    }

    void InputService::UnregisterMouseClickCallback(const DelegateHandle<int, int>& handle)
    {
        mouseClickEventCallbacks.UnregisterCallback(handle);
    }

    DelegateHandle<> InputService::RegisterMouseReleaseCallback(MouseReleaseEventCallback callback)
    {
        return mouseReleaseEventCallbacks.RegisterCallback(callback);
    }

    void InputService::UnregisterMouseReleaseCallback(const DelegateHandle<>& callback)
    {
        mouseReleaseEventCallbacks.UnregisterCallback(callback);
    }

    DelegateHandle<int, int> InputService::RegisterMouseRightClickCallback(const MouseClickEventCallback& callback)
    {
        return mouseRightClickEventCallbacks.RegisterCallback(callback);
    }

    void InputService::UnregisterMouseRightClickCallback(const DelegateHandle<int, int>& handle)
    {
        mouseRightClickEventCallbacks.UnregisterCallback(handle);
    }

    DelegateHandle<> InputService::RegisterMouseRightReleaseCallback(const MouseReleaseEventCallback& callback)
    {
        return mouseRightReleaseEventCallbacks.RegisterCallback(callback);
    }

    void InputService::UnregisterMouseRightReleaseCallback(const DelegateHandle<>& callback)
    {
        mouseRightReleaseEventCallbacks.UnregisterCallback(callback);
    }

    glm::vec2 InputService::GetInputAxisNormalized(const KeyboardInputAxis2D input) const
    {
        glm::vec2 result = glm::vec2(keyPressed[input.xPos] - keyPressed[input.xNeg], keyPressed[input.yPos] - keyPressed[input.yNeg]);
        float lengthSquared = result.x * result.x + result.y * result.y;
        if (lengthSquared > 1.0f)
        {
            float length = std::sqrtf(lengthSquared);
            result.x /= length;
            result.y /= length;
        }
        return result;
    }

    float InputService::GetInputAxis(const KeyboardInputAxis input) const
    {
        return keyPressed[input.positive] - keyPressed[input.negative];
    }

    glm::vec2 InputService::GetMousePosition() const
    {
        float x, y;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }

    bool Vox::InputService::ShouldCloseWindow() const
    {
        return windowClosed;
    }

    void InputService::ToggleCursorLock()
    {
        if (!cursorLocked)
        {
            SDL_HideCursor();
            SDL_SetWindowRelativeMouseMode(mainWindow, true);
            cursorLocked = true;
        }
        else
        {
            SDL_ShowCursor();
            SDL_SetWindowRelativeMouseMode(mainWindow, false);
            cursorLocked = false;
        }
    }

    bool InputService::IsWindowFullscreen() const
    {
        return windowFullscreen;
    }

    bool InputService::IsWindowMaximized() const
    {
        return windowMaximized;
    }

    void InputService::HandleEvent(SDL_Event* event)
    {
        switch (event->type)
        {
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_WINDOW_RESIZED:
        {
            HandleWindowEvent(event->window);
            return;
        }

        case SDL_EVENT_KEY_DOWN:
        {
            SDL_Scancode pressedKeyCode = event->key.scancode;
            if (!keyPressed[pressedKeyCode])
            {
                keyPressed[pressedKeyCode] = true;
                // TraceLog(LOG_INFO, TextFormat("Key %i pressed", pressedKeyCode));
                ExecuteCallbacks(pressedKeyCode, true);
            }
            return;
        }

        case SDL_EVENT_KEY_UP:
        {
            SDL_Scancode pressedKeyCode = event->key.scancode;
            if (keyPressed[pressedKeyCode])
            {
                keyPressed[pressedKeyCode] = false;
                // TraceLog(LOG_INFO, TextFormat("Key %i released", pressedKeyCode));
                ExecuteCallbacks(pressedKeyCode, false);
            }
            return;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            HandleMouseMotionEvent(event->motion);
            return;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (!ImGui::GetIO().WantCaptureMouse)
            {
                HandleMouseButtonEvent(event->button);
            }
            return;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            HandleMouseButtonReleaseEvent(event->button);
            return;
        }
        default:
        {

        }
        }
    }

    void InputService::HandleWindowEvent(SDL_WindowEvent& windowEvent)
    {
        switch (windowEvent.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
        {
            SDL_SetWindowSize(SDL_GetWindowFromID(windowEvent.windowID), windowEvent.data1, windowEvent.data2);

            glViewport(0, 0, windowEvent.data1, windowEvent.data2);
            return;
        }

        case SDL_EVENT_WINDOW_RESTORED:
        {
            SDL_RestoreWindow(SDL_GetWindowFromID(windowEvent.windowID));
            windowFullscreen = false;
            windowMaximized = false;
            return;
        }

        case SDL_EVENT_WINDOW_MAXIMIZED:
        {
            SDL_MaximizeWindow(SDL_GetWindowFromID(windowEvent.windowID));
            windowMaximized = true;
            return;
        }

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            windowClosed = true;
            return;
        }

        default:
        {
            return;
        }
        }
    }

    void InputService::HandleMouseMotionEvent(SDL_MouseMotionEvent& mouseEvent)
    {
        if (!cursorLocked)
        {
            return;
        }

        mouseMotionEventCallbacks(mouseEvent.xrel, mouseEvent.yrel);
    }

    void InputService::HandleMouseButtonEvent(SDL_MouseButtonEvent& mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_LEFT)
        {
            mouseClickEventCallbacks(mouseEvent.x, mouseEvent.y);
            return;
        }

        if (mouseEvent.button == SDL_BUTTON_RIGHT)
        {
            mouseRightClickEventCallbacks(mouseEvent.x, mouseEvent.y);
            return;
        }
    }

    void InputService::HandleMouseButtonReleaseEvent(SDL_MouseButtonEvent& mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_LEFT)
        {
            mouseReleaseEventCallbacks();
            return;
        }

        if (mouseEvent.button == SDL_BUTTON_RIGHT)
        {
            mouseRightReleaseEventCallbacks();
            return;
        }
    }

    void InputService::ExecuteCallbacks(SDL_Scancode scancode, bool pressed)
    {
        SDL_Scancode pressedKeyCode = scancode;
        auto callbacks = keyboardEventMap.find(pressedKeyCode);
        if (callbacks == keyboardEventMap.end())
        {
            return;
        }

        callbacks->second(pressed);
    }

    void InputService::ToggleFullscreen()
    {
        if (windowFullscreen)
        {
            SDL_SetWindowFullscreen(mainWindow, false);
            windowFullscreen = false;
        }
        else
        {
            SDL_SetWindowFullscreen(mainWindow, true);
            windowFullscreen = true;
        }
    }

    KeyboardInputAxis::KeyboardInputAxis(SDL_Scancode positive, SDL_Scancode negative)
        :positive(positive), negative(negative)
    {
    }

    KeyboardInputAxis2D::KeyboardInputAxis2D(SDL_Scancode yPos, SDL_Scancode yNeg, SDL_Scancode xPos, SDL_Scancode xNeg)
        :yPos(yPos), yNeg(yNeg), xPos(xPos), xNeg(xNeg)
    {
    }
}