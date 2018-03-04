#pragma once

class RLEntity;
void UpdateCameraOffset(RLEntity* cameraTrackingEntity, int& camXOffset, int& camYOffset);

void DrawSidebar();

// Returns whether it should restart or not
bool PlayGame();