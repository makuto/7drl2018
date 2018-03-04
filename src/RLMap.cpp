#include "RLMap.hpp"

#include "Game.hpp"
#include "Utilities.hpp"

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

RLMap::RLMap(int width, int height) : Width(width), Height(height)
{
	Tiles.resize(width * height);

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
	for (int camY = 0; camY < MIN(map.Height, ViewTileHeight); ++camY)
	{
		for (int camX = 0; camX < MIN(map.Width, ViewTileWidth); ++camX)
		{
			int tileX = camX + camXOffset;
			int tileY = camY + camYOffset;
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
				displayText.setText(buffer);
				displayText.setPosition(TileTextWidth * camX, TileTextHeight * camY);
				win.draw(&displayText);
			}
		}
	}
}