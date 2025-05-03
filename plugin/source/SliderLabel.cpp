/*
  ==============================================================================

    SliderLabel.cpp
    Created: 7 Jun 2021 7:19:58pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SliderLabel.h"

RotaryLabel::RotaryLabel (juce::Slider* s, int decPlaces) : linkedSlider (s), decimalPlaces (decPlaces)
{
    linkedSlider->addListener (this);
    setEditable (true);
    auto str = juce::String (linkedSlider->getValue());
    if (str.length() > decimalPlaces)
        str = str.substring (0, decimalPlaces);
    setText (str, juce::dontSendNotification);
}

void RotaryLabel::sliderValueChanged (juce::Slider *s)
{
    auto str = juce::String (linkedSlider->getValue());
    if (str.length() > decimalPlaces)
        str = str.substring (0, decimalPlaces);
    setText (str, juce::dontSendNotification);
}

juce::String RotaryLabel::currentValueString()
{
    auto str = juce::String (linkedSlider->getValue());
    if (str.length() > decimalPlaces)
        str = str.substring (0, decimalPlaces);
    return str;
}

void RotaryLabel::componentMovedOrResized (juce::Component &component, bool wasMoved, bool wasResized)
{
    auto& lf = getLookAndFeel();
    auto f = lf.getLabelFont (*this);
    auto borderSize = lf.getLabelBorderSize (*this);

    if(isAttachedOnLeft())
    {
        auto width = fmin (juce::roundToInt (f.getStringWidthFloat (getText()) * 3.5f)
                             + borderSize.getLeftAndRight(),
                           component.getX());

        setBounds (component.getX() - width, component.getY(), width, component.getHeight());
    }
    else
    {
        auto height = borderSize.getTopAndBottom() + 2 + juce::roundToInt (f.getHeight() * 1.5f);
        auto stringWidth = f.getStringWidthFloat (currentValueString());
        auto width = juce::roundToInt (stringWidth) + borderSize.getLeftAndRight();
        auto centerX = component.getX() + juce::roundToInt (component.getWidth() / 2.0f);
        auto x = centerX - juce::roundToInt (width / 2.0f);
        setBounds (x, component.getBottom() + 1, width, height);
    }
}



void RotaryParamName::componentMovedOrResized (juce::Component &component, bool wasMoved, bool wasResized)
{
    auto& lf = getLookAndFeel();
    auto f = lf.getLabelFont (*this);
    auto borderSize = lf.getLabelBorderSize (*this);

    if(isAttachedOnLeft())
    {
        auto width = fmin (juce::roundToInt (f.getStringWidthFloat (text) * 3.5f)
                             + borderSize.getLeftAndRight(),
                           component.getX());

        setBounds (component.getX() - width, component.getY(), width, component.getHeight());
    }
    else
    {
        auto height = borderSize.getTopAndBottom() + 2 + juce::roundToInt (f.getHeight() * 1.5f);
        auto width = juce::roundToInt (f.getStringWidthFloat (text)) + borderSize.getLeftAndRight();
        auto dX = (component.getWidth() - width) / 2;
        auto gap = juce::roundToInt (component.getHeight() * fLift);
        setBounds (component.getX() + dX, component.getY() - height - gap, width, height);
    }
}

void VerticalParamName::componentMovedOrResized (juce::Component &component, bool wasMoved, bool wasResized)
{
    auto& lf = getLookAndFeel();
    auto f = lf.getLabelFont (*this);
    auto borderSize = lf.getLabelBorderSize (*this);

    if(isAttachedOnLeft())
    {
        auto width = fmin (juce::roundToInt(f.getStringWidthFloat (text) * 3.5f)
                             + borderSize.getLeftAndRight(),
                           component.getX());

        setBounds (component.getX() - width, component.getY(), width, component.getHeight());
    }
    else
    {
        auto height = borderSize.getTopAndBottom() + 2 + juce::roundToInt (f.getHeight() * 1.5f);
        auto width = juce::roundToInt (f.getStringWidthFloat (text)) + borderSize.getLeftAndRight();
        auto xCenter = component.getX() + (component.getWidth() / 2);
        setBounds (xCenter - (width / 2), component.getBottom(), width, height);
    }
}
