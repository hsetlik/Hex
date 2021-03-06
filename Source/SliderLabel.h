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
#include "CustomLnF.h"
class RotaryLabel : public juce::Label, public juce::Slider::Listener
{
public:
    RotaryLabel (juce::Slider* s, int decimals = 5);
    juce::Slider* const linkedSlider;
    void textWasEdited() override
    {
        auto str = getText (true);
        auto val = str.getDoubleValue();
        linkedSlider->setValue (val);
    }
    juce::String currentValueString();
    void sliderValueChanged (juce::Slider* s) override;
    void setDecimalPlaces (int numPlaces) {decimalPlaces = numPlaces; }
    void componentMovedOrResized (juce::Component& comp, bool wasMoved, bool wasResized) override;
private:
    int decimalPlaces;
};

class ParamName : public juce::Label
//! label subclass with non-editable text. use \c placeRelative in the parent's \c resized() method to position label relative to some slider/button
{
public:
    ParamName (juce::String name) : text (name)
    {
        setText (text, juce::dontSendNotification);
        setEditable (false);
        setMinimumHorizontalScale (0.01f);
        setLookAndFeel (&lnf);
    }
    virtual ~ParamName() {setLookAndFeel (nullptr); }
    virtual void placeRelative (juce::Component& attach, int heightFraction, int gapFraction, bool isAbove) {}
protected:
    juce::String text;
private:
    HexLookAndFeel lnf;
};

class RotaryParamName : public ParamName
//! Parameter
{
public:
    RotaryParamName (juce::String name) : ParamName (name), fLift (0.05f)
    {
        setJustificationType (juce::Justification::centredBottom);
    }
    void componentMovedOrResized (juce::Component& comp, bool wasMoved, bool wasResized) override;
    void setLift (float value) {fLift = value; }
private:
    float fLift;
};

class VerticalParamName : public ParamName
{
public:
    VerticalParamName (juce::String name) : ParamName (name), fLift (0.02f)
    {
        setJustificationType (juce::Justification::centredTop);
    }
    void componentMovedOrResized (juce::Component& comp, bool wasMoved, bool wasResized) override;
    void setLift (float value) {fLift = value; }
private:
    float fLift;
};

