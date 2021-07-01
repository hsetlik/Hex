/*
  ==============================================================================

    CustomLnF.h
    Created: 23 Jun 2021 12:05:42am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "ComponentUtil.h"

class HexLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Font getLabelFont(juce::Label& label) override;
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    void drawRotarySlider (juce::Graphics &g, int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider &slider) override;
};
