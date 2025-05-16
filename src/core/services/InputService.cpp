#include "InputService.h"

#include <cmath>

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
        RegisterKeyboardCallback(SDL_SCANCODE_ESCAPE, [this](bool pressed) { windowClosed = true; });
        RegisterKeyboardCallback(SDL_SCANCODE_TAB, [this](bool pressed) { if (pressed) ToggleCursorLock(); });
        RegisterKeyboardCallback(SDL_SCANCODE_F11, [this](bool pressed) { if (pressed) ToggleFullscreen(); });
    }

    InputService::~InputService()
    = default;

    void InputService::PollEvents()
    {
        SDL_Event polledEvent;
        while (SDL_PollEvent(&polledEvent))
        {
            HandleEvent(&polledEvent);
            ImGui_ImplSDL3_ProcessEvent(&polledEvent);

        }
    }

    const KeyboardEventCallback& InputService::RegisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback)
    {
        auto callbacks = keyboardEventMap.find(scancode);
        if (callbacks == keyboardEventMap.end())
        {
            std::vector<KeyboardEventCallback> newCallbackVector{ callback };
            auto [fst, snd] = keyboardEventMap.insert(std::pair<SDL_Scancode, std::vector<KeyboardEventCallback>>(scancode, newCallbackVector));
            return fst->second.at(0);
        }

        return callbacks->second.emplace_back(std::move(callback));
    }

    void InputService::UnregisterKeyboardCallback(SDL_Scancode scancode, const KeyboardEventCallback& callback)
    {
        auto callbacks = keyboardEventMap.find(scancode);
        if (callbacks == keyboardEventMap.end())
        {
            return;
        }

        for (int i = 0; i < callbacks->second.size(); ++i)
        {
            if (callbacks->second[i].target<void(bool)>() == callback.target<void(bool)>())
            {
                callbacks->second.erase(callbacks->second.begin() + i);
                return;
            }
        }
    }

    const MouseMotionEventCallback& InputService::RegisterMouseMotionCallback(MouseMotionEventCallback callback)
    {
        return mouseMotionEventCallbacks.emplace_back(std::move(callback));
    }

    void InputService::UnregisterMouseMotionCallback(const MouseMotionEventCallback& callback)
    {
        for (int i = 0; i < mouseMotionEventCallbacks.size(); ++i)
        {
            if (mouseMotionEventCallbacks[i].target<void(bool)>() == callback.target<void(bool)>())
            {
                mouseMotionEventCallbacks.erase(mouseMotionEventCallbacks.begin() + i);
                return;
            }
        }
    }

    const MouseMotionEventCallback& InputService::RegisterMouseClickCallback(MouseClickEventCallback callback)
    {
        return mouseClickEventCallbacks.emplace_back(std::move(callback));
    }

    void InputService::UnregisterMouseClickCallback(const MouseClickEventCallback& callback)
    {
        auto target = callback.target<void(int, int)>();
        std::erase_if(mouseClickEventCallbacks, [target](const MouseClickEventCallback& clickCallback)
        {
           return clickCallback.target<void(int, int)>() == target;
        });
    }

    const MouseReleaseEventCallback& InputService::RegisterMouseReleaseCallback(MouseReleaseEventCallback callback)
    {
        return mouseReleaseEventCallbacks.emplace_back(std::move(callback));
    }

    void InputService::UnregisterMouseReleaseCallback(const MouseReleaseEventCallback& callback)
    {
        auto target = callback.target<void()>();
        std::erase_if(mouseReleaseEventCallbacks, [target](const MouseReleaseEventCallback& clickCallback)
        {
           return clickCallback.target<void()>() == target;
        });
    }

    glm::vec2 InputService::GetInputAxisNormalized(KeyboardInputAxis2D input) const
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

        for (MouseMotionEventCallback& callback : mouseMotionEventCallbacks)
        {
            callback(mouseEvent.xrel, mouseEvent.yrel);
        }
    }

    void InputService::HandleMouseButtonEvent(SDL_MouseButtonEvent& mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_LEFT)
        {
            // VoxLog(Display, Input, "Mouse clicked at position({}, {})", mouseEvent.x, mouseEvent.y);
            for (MouseClickEventCallback& callback : mouseClickEventCallbacks)
            {
                callback(mouseEvent.x, mouseEvent.y);
            }
        }
    }

    void InputService::HandleMouseButtonReleaseEvent(SDL_MouseButtonEvent& mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_LEFT)
        {
            // VoxLog(Display, Input, "Mouse released at position({}, {})", mouseEvent.x, mouseEvent.y);
            for (MouseReleaseEventCallback& callback : mouseReleaseEventCallbacks)
            {
                callback();
            }
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

        for (KeyboardEventCallback& callback : callbacks->second)
        {
            callback(pressed);
        }
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

    KeyboardInputAxis2D::KeyboardInputAxis2D(SDL_Scancode yPos, SDL_Scancode yNeg, SDL_Scancode xPos, SDL_Scancode xNeg)
        :yPos(yPos), yNeg(yNeg), xPos(xPos), xNeg(xNeg)
    {
    }
}