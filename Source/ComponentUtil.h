/*
  ==============================================================================

    ComponentUtil.h
    Created: 7 Jun 2021 3:48:31pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SliderUtil
{
public:
    static void setVerticalLinearNoBox(juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 1, 1);
    }
};
