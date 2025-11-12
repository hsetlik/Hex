/*
  ==============================================================================

    Color.h
    Created: 4 Jun 2021 1:24:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "HexHeader.h"
#include "FontData.h"
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

static juce::Font robotoLightItalic =
    juce::Font(juce::FontOptions(juce::Typeface::createSystemTypefaceFor(
        FontData::RobotoLightItalic_ttf,
        FontData::RobotoLightItalic_ttfSize)));
static juce::Font robotoThin =
    juce::Font(juce::FontOptions(juce::Typeface::createSystemTypefaceFor(
        FontData::RobotoLightItalic_ttf,
        FontData::RobotoLightItalic_ttfSize)));
}  // namespace UXPalette
