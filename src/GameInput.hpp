#pragma once

struct GameInput
{
	inputManager& inp;
	std::map<inputCode::keyCode, bool> keyTapStates;

	GameInput(inputManager& newInput) : inp(newInput)
	{
	}

	bool Tapped(inputCode::keyCode key)
	{
		bool isPressed = inp.isPressed(key);

		std::map<inputCode::keyCode, bool>::iterator findIt = keyTapStates.find(key);
		if (findIt != keyTapStates.end())
		{
			if (isPressed)
			{
				if (findIt->second)
				{
					// Wasn't tapped this frame
					return false;
				}
				else
				{
					findIt->second = true;
					return true;
				}
			}
			else
				findIt->second = false;
		}
		else
		{
			keyTapStates[key] = true;
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
