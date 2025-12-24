#pragma once
#include "HexHeader.h"
#include "Identifiers.h"

namespace Fonts {
enum FontID {
  RobotoLight,
  RobotoLightItalic,
  RobotoMediumItalic,
  RobotoBlackItalic,
  KenyanBoldItalic,
  KenyanReg
};

juce::Font getFont(const FontID& id, float size = 12.0f);
}  // namespace Fonts
