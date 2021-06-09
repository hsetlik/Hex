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
    static void setRotaryNoBox(juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 1, 1);
    }
};

class ComponentUtil
{
public:
    static juce::Rectangle<int> boxBelow(juce::Component& parent, int heightFraction)
    {
        int x, y, width, height;
        auto iBounds = parent.getBounds();
        height = iBounds.getHeight() / heightFraction;
        width = iBounds.getWidth();
        x = iBounds.getX();
        y = iBounds.getBottom();
        return juce::Rectangle<int>(x, y, width, height);
    }
};
