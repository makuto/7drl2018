#pragma once

typedef char RLDrawableType;

struct RLDrawable
{
	int X;
	int Y;

	RLDrawableType Type;
};

struct Player : RLDrawable
{
	Player();
};