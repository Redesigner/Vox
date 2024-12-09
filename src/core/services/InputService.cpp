#include "InputService.h"

#include "raylib.h"
#include "rlgl.h"
#include <SDL2/SDL_events.h>
#include <cmath>

Vox::InputService::InputService()
{
	windowClosed = false;
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

void Vox::InputService::RegisterCallback(SDL_Scancode scancode, KeyboardEventCallback callback)
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

void Vox::InputService::UnregisterCallback(SDL_Scancode scancode, KeyboardEventCallback callback)
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
				TraceLog(LOG_INFO, TextFormat("Key %i pressed", pressedKeyCode));
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
				TraceLog(LOG_INFO, TextFormat("Key %i released", pressedKeyCode));
				ExecuteCallbacks(pressedKeyCode, false);
			}
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
