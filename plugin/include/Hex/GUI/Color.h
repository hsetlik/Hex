/*
  ==============================================================================

    Color.h
    Created: 4 Jun 2021 1:24:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "HexHeader.h"

namespace UXPalette {
static Color thumbYellow = Color(255, 236, 95);
static Color thumbBlue = Color(32, 139, 181);
static Color highlight = Color(255, 236, 95);
static Color darkBkgnd = juce::Colours::black.brighter(0.1f);
static Color darkGray = Color(55, 56, 68);
static Color lightGray = Color(125, 126, 129);
static Color lightRed = Color(226, 76, 85);
static Color darkRed = lightRed.darker(0.45f);
static Color darkBlue = Color(39, 104, 158);
static Color lightBlue = Color(91, 201, 225);

}  // namespace UXPalette

#define DECLARE_COLOR(name, r, g, b) const Color name = Color::fromRGB(r, g, b);

namespace UIColor {
DECLARE_COLOR(orangeLight, 226, 79, 54)
DECLARE_COLOR(greenLight, 128, 254, 85)
DECLARE_COLOR(bkgndGray, 51, 51, 51)
DECLARE_COLOR(shadowGray, 25, 25, 25)
DECLARE_COLOR(borderGray, 102, 102, 102)
DECLARE_COLOR(offWhite, 204, 204, 204)

}  // namespace UIColor

#undef DECLARE_COLOR
