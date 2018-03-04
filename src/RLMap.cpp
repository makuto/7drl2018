#include "RLMap.hpp"

#include "Game.hpp"
#include "Utilities.hpp"

#include "math/math.hpp"

std::string GetTileDescription(RLTile& tile)
{
	std::string description = "";
	switch (tile.Type)
	{
		case '#':
			description += "a wall";
			break;
		case '.':
			description += "a stone tile";
			break;
		default:
			description += "something you don't understand";
			break;
	}

	return description;
}

RLMap::RLMap() : Width(128), Height(128)
{
	Tiles.resize(Width * Height);

	for (RLTile& tile : Tiles)
	{
		tile.Type = FLOOR_TYPE;
		tile.Color = {FLOOR_TILE_COLOR_NORMAL};
	}
}

RLMap::RLMap(int width, int height) : Width(width), Height(height)
{
	Tiles.resize(Width * Height);

	for (RLTile& tile : Tiles)
	{
		tile.Type = FLOOR_TYPE;
		tile.Color = {FLOOR_TILE_COLOR_NORMAL};
	}
}

RLTile* RLMap::At(int x, int y)
{
	unsigned int index = y * Width + x;
	if (x < 0 || y < 0 || x >= Width || y >= Width || index >= Tiles.size())
		return nullptr;
	return &Tiles[index];
}

void DrawWorld(RLMap& map, int camXOffset, int camYOffset)
{
	for (int viewY = 0; viewY < ViewTileHeight; ++viewY)
	{
		for (int viewX = 0; viewX < ViewTileWidth; ++viewX)
		{
			const float fadeNoDisplay = 30.f;
			const float fadeScale = 1.f;

			int tileX = viewX + camXOffset;
			int tileY = viewY + camYOffset;

			float distanceFromPlayer =
			    distanceTo(tileX, tileY, gameState.player.X, gameState.player.Y);
			if (distanceFromPlayer > fadeNoDisplay)
				continue;

			RLTile* currentTile = map.At(tileX, tileY);

			static std::string buffer = "";
			buffer.clear();

			bool shouldDrawTile = true;
			for (RLEntity* npc : gameState.npcs)
			{
				if (npc->X == tileX && npc->Y == tileY && !gameState.player.SamePos(*npc))
				{
					buffer += npc->Type;
					SetTextColor(displayText, npc->Color);

					shouldDrawTile = false;
					break;
				}
			}

			if (buffer.empty() && gameState.player.X == tileX && gameState.player.Y == tileY)
			{
				buffer += gameState.player.Type;
				SetTextColor(displayText, gameState.player.Color);
			}
			else if (buffer.empty())
			{
				if (currentTile)
				{
					buffer += currentTile->Type;
					SetTextColor(displayText, currentTile->Color);
				}
			}

			if (!buffer.empty())
			{
				// Don't affect the opacity of monsters
				if (!std::isalnum(buffer[0]))
				{
					float fadeDistance = distanceFromPlayer * fadeScale;
					CLAMP(fadeDistance, 0.f, 100.f);
					// char newAlpha = 255 / (fadeDistance * fadeDistance * fadeDistance);
					distanceFromPlayer /= 2.f;
					CLAMP(distanceFromPlayer, 0.f, 254.f);
					char newAlpha = 255 - (distanceFromPlayer * distanceFromPlayer);
					displayText.setAlpha(newAlpha);
				}

				displayText.setText(buffer);
				displayText.setPosition(TileTextWidth * viewX, TileTextHeight * viewY);
				win.draw(&displayText);
			}
		}
	}
}