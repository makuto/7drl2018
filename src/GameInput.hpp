#pragma once

struct GameInput
{
	struct InputState
	{
		bool ProcessedTapped;
		float KeyRepeatTime;
	};

	std::map<inputCode::keyCode, InputState> keyTapStates;

	inputManager& inp;
	timer repeatTimer;

	float KeyRepeatPauseTime;
	float KeyRepeatTimePerRepeat;

	GameInput(inputManager& newInput) : inp(newInput)
	{
		repeatTimer.start();
		KeyRepeatPauseTime = .500f;
		KeyRepeatTimePerRepeat = .1f;
	}

	bool UpdateRepeat(InputState& state, bool isPressed)
	{
		float currentTime = repeatTimer.getTime();
		if (isPressed)
		{
			if (state.KeyRepeatTime)
			{
				if (state.KeyRepeatTime <= currentTime)
				{
					state.KeyRepeatTime = currentTime + KeyRepeatTimePerRepeat;
					return true;
				}
				else
					return false;
			}
			else
			{
				state.KeyRepeatTime = currentTime + KeyRepeatPauseTime;
				return false;
			}
		}

		state.KeyRepeatTime = 0.f;

		return false;
	}

	bool Tapped(inputCode::keyCode key)
	{
		bool isPressed = inp.isPressed(key);

		std::map<inputCode::keyCode, InputState>::iterator findIt = keyTapStates.find(key);
		if (findIt != keyTapStates.end())
		{
			bool shouldRepeat = UpdateRepeat(findIt->second, isPressed);
			if (isPressed)
			{
				if (findIt->second.ProcessedTapped)
				{
					if (shouldRepeat)
						return true;
					// Wasn't tapped this frame
					return false;
				}
				else
				{
					findIt->second.ProcessedTapped = true;
					return true;
				}
			}
			else
			{
				findIt->second.ProcessedTapped = false;
				findIt->second.KeyRepeatTime = 0.f;
			}
		}
		else
		{
			keyTapStates[key] = {true, 0.f};
			return isPressed;
		}

		return false;
	}

	void GetInpDirectionalNavigation(int& deltaX, int& deltaY)
	{
		if (Tapped(inputCode::Up) || Tapped(inputCode::Numpad8))
			deltaY = -1;
		if (Tapped(inputCode::Down) || Tapped(inputCode::Numpad2))
			deltaY = 1;
		if (Tapped(inputCode::Left) || Tapped(inputCode::Numpad4))
			deltaX = -1;
		if (Tapped(inputCode::Right) || Tapped(inputCode::Numpad6))
			deltaX = 1;
		if (Tapped(inputCode::Numpad9))
		{
			deltaX = 1;
			deltaY = -1;
		}
		if (Tapped(inputCode::Numpad3))
		{
			deltaX = 1;
			deltaY = 1;
		}
		if (Tapped(inputCode::Numpad1))
		{
			deltaX = -1;
			deltaY = 1;
		}
		if (Tapped(inputCode::Numpad7))
		{
			deltaX = -1;
			deltaY = -1;
		}
	}
};
