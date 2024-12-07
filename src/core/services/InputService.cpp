#include "InputService.h"

#include <SDL2/SDL_events.h>

Vox::InputService::InputService()
{
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

void Vox::InputService::HandleEvent(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_KEYDOWN:
		{
			SDL_Scancode pressedKeyCode = event->key.keysym.scancode;
			ExecuteCallbacks(pressedKeyCode, true);
			return;
		}

		case SDL_KEYUP:
		{
			SDL_Scancode pressedKeyCode = event->key.keysym.scancode;
			ExecuteCallbacks(pressedKeyCode, false);
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
