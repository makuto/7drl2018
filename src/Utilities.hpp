#pragma once

void ConsoleAndGameLogOutput(const gv::Logging::Record& record);

void RecalculateDimensions();

class text;
struct RLColor;
void SetTextColor(text& text, RLColor& color);