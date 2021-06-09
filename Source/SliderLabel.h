/*
  ==============================================================================

    SliderLabel.h
    Created: 7 Jun 2021 7:19:58pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "ComponentUtil.h"

class RotaryLabel : public juce::Label, public juce::Slider::Listener
{
public:
    RotaryLabel(juce::Slider* s, int decimals = 5);
    juce::Slider* const linkedSlider;
    void textWasEdited() override
    {
        auto str = getText(true);
        auto val = str.getDoubleValue();
        linkedSlider->setValue(val);
    }
    void sliderValueChanged(juce::Slider* s) override;
    void resized() override;
    void setDecimalPlaces(int numPlaces) {decimalPlaces = numPlaces; }
private:
    int decimalPlaces;
};
