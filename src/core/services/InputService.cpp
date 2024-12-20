#include "InputService.h"

#include "raylib.h"
#include "rlgl.h"
#include <SDL2/SDL_events.h>
#include <cmath>

Vox::InputService::InputService()
{
	ToggleCursorLock();
	windowClosed = false;

	RegisterKeyboardCallback(SDL_SCANCODE_ESCAPE, [this](bool pressed) { windowClosed = true; });
	RegisterKeyboardCallback(SDL_SCANCODE_TAB, [this](bool pressed) { if (pressed) ToggleCursorLock(); });
}

Vox::InputService::~InputService()
{
}

void Vox::InputService::PollEvents()
{
	SDL_Event polledEvent;
	while (SDL_PollEvent(&polledEvent))
	{
		HandleEvent(&polledEvent);
	}
}

void Vox::InputService::RegisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback)
{
	auto callbacks = keyboardEventMap.find(scancode);
	if (callbacks == keyboardEventMap.end())
	{
		std::vector<KeyboardEventCallback> newCallbackVector{ callback };
		keyboardEventMap.insert(std::pair<SDL_Scancode, std::vector<KeyboardEventCallback>>(scancode, newCallbackVector));
		return;
	}

	callbacks->second.push_back(callback);
}

void Vox::InputService::UnregisterKeyboardCallback(SDL_Scancode scancode, KeyboardEventCallback callback)
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

void Vox::InputService::RegisterMouseMotionCallback(MouseMotionEventCallback callback)
{
	mouseMotionEventCallbacks.push_back(callback);
}

void Vox::InputService::UnregisterMouseMotionCallback(MouseMotionEventCallback callback)
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

void Vox::InputService::RegisterMouseClickCallback(MouseClickEventCallback callback)
{
	mouseClickEventCallbacks.push_back(callback);
}

void Vox::InputService::UnregisterMouseClickCallback(MouseClickEventCallback callback)
{
	for (int i = 0; i < mouseClickEventCallbacks.size(); ++i)
	{
		if (mouseClickEventCallbacks[i].target<void(bool)>() == callback.target<void(bool)>())
		{
			mouseClickEventCallbacks.erase(mouseClickEventCallbacks.begin() + i);
			return;
		}
	}
}

Vector2 Vox::InputService::GetInputAxisNormalized(KeyboardInputAxis2D input) const
{
	Vector2 result = Vector2(keyPressed[input.xPos] - keyPressed[input.xNeg], keyPressed[input.yPos] - keyPressed[input.yNeg]);
	float lengthSquared = result.x * result.x + result.y * result.y;
	if (lengthSquared > 1.0f)
	{
		float length = std::sqrtf(lengthSquared);
		result.x /= length;
		result.y /= length;
	}
	return result;
}

bool Vox::InputService::ShouldCloseWindow() const
{
	return windowClosed;
}

void Vox::InputService::ToggleCursorLock()
{
	if (!cursorLocked)
	{
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
		cursorLocked = true;
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		cursorLocked = false;
	}
}

void Vox::InputService::HandleEvent(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_WINDOWEVENT:
		{
			HandleWindowEvent(event->window);
			return;
		}

		case SDL_KEYDOWN:
		{
			SDL_Scancode pressedKeyCode = event->key.keysym.scancode;
			if (!keyPressed[pressedKeyCode])
			{
				keyPressed[pressedKeyCode] = true;
				// TraceLog(LOG_INFO, TextFormat("Key %i pressed", pressedKeyCode));
				ExecuteCallbacks(pressedKeyCode, true);
			}
			return;
		}

		case SDL_KEYUP:
		{
			SDL_Scancode pressedKeyCode = event->key.keysym.scancode;
			if (keyPressed[pressedKeyCode])
			{
				keyPressed[pressedKeyCode] = false;
				// TraceLog(LOG_INFO, TextFormat("Key %i released", pressedKeyCode));
				ExecuteCallbacks(pressedKeyCode, false);
			}
			return;
		}

		case SDL_MOUSEMOTION:
		{
			HandleMouseMotionEvent(event->motion);
			return;
		}

		case SDL_MOUSEBUTTONDOWN:
		{
			HandleMouseButtonEvent(event->button);
			return;
		}

		default:
		{
			return;
		}
	}
}

void Vox::InputService::HandleWindowEvent(SDL_WindowEvent& windowEvent)
{
	switch (windowEvent.event)
	{
		case SDL_WINDOWEVENT_RESIZED:
		{
			SetWindowSize(windowEvent.data1, windowEvent.data2);
			rlViewport(0, 0, windowEvent.data1, windowEvent.data2);

			rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
			rlLoadIdentity();                   // Reset current matrix (projection)

			// Set orthographic projection to current framebuffer size
			// NOTE: Configured top-left corner as (0, 0)
			rlOrtho(0, windowEvent.data1, windowEvent.data2, 0, 0.0f, 1.0f);

			rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
			rlLoadIdentity();                   // Reset current matrix (modelview)
			return;
		}

		case SDL_WINDOWEVENT_CLOSE:
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

void Vox::InputService::HandleMouseMotionEvent(SDL_MouseMotionEvent& mouseEvent)
{
	for (MouseMotionEventCallback& callback : mouseMotionEventCallbacks)
	{
		callback(mouseEvent.xrel, mouseEvent.yrel);
	}
}

void Vox::InputService::HandleMouseButtonEvent(SDL_MouseButtonEvent& mouseEvent)
{
	if (mouseEvent.button == SDL_BUTTON_LEFT)
	{
		TraceLog(LOG_INFO, TextFormat("[InputService] Mouse clicked at position (%i, %i)", mouseEvent.x, mouseEvent.y));
		for (MouseClickEventCallback& callback : mouseClickEventCallbacks)
		{
			callback(mouseEvent.x, mouseEvent.y);
		}
	}
}

void Vox::InputService::ExecuteCallbacks(SDL_Scancode scancode, bool pressed)
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

Vox::KeyboardInputAxis2D::KeyboardInputAxis2D(SDL_Scancode yPos, SDL_Scancode yNeg, SDL_Scancode xPos, SDL_Scancode xNeg)
	:yPos(yPos), yNeg(yNeg), xPos(xPos), xNeg(xNeg)
{
}
