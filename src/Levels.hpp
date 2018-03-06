#pragma once

typedef struct RLMap RLMap;
typedef class RLEntity RLEntity;

void placeEntityWithinSquareRandomSensibly(RLEntity* entity, int xCenter, int yCenter, int radius);
void placeEntityRandomSensibly(RLEntity* entity);

void createTestMap(RLMap& map);
void createTestMapNoise(RLMap& map);

void ClearNpcs();

void LoadNextLevel();