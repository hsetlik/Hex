/*
  ==============================================================================

    SliderLabel.cpp
    Created: 7 Jun 2021 7:19:58pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SliderLabel.h"

RotaryLabel::RotaryLabel(juce::Slider* s, int decPlaces) : linkedSlider(s), decimalPlaces(decPlaces)
{
    linkedSlider->addListener(this);
    setEditable(true);
    auto str = juce::String(linkedSlider->getValue());
    if(str.length() > decimalPlaces)
        str = str.substring(0, decimalPlaces);
    setText(str, juce::dontSendNotification);
}

void RotaryLabel::sliderValueChanged(juce::Slider *s)
{
    auto str = juce::String(linkedSlider->getValue());
    if(str.length() > decimalPlaces)
        str = str.substring(0, decimalPlaces);
    setText(str, juce::dontSendNotification);
}

void RotaryLabel::resized()
{
    setBounds(ComponentUtil::boxBelow(*linkedSlider, 6, 10));
    auto height = (float)getHeight() * 1.5f;
    setFont(getFont().withHeight(height));
}

void RotaryParamName::placeRelative(juce::Component &attach, int heightFraction, int gapFraction, bool isAbove)
{
    int x, y, width, height;
    auto bounds = attach.getBounds();
    height = bounds.getHeight() / heightFraction;
    auto font = getFont().withHeight((float)height);
    width = font.getStringWidth(text);
    printf("Width for string \"%s\" is %d\n", text.toRawUTF8(), width);
    x = bounds.getX() + ((bounds.getWidth() - width) / 2);
    int gap = bounds.getHeight() / gapFraction;
    y = (isAbove)? bounds.getY() - (height + gap) : bounds.getBottom() + gap;
    setFont(font);
    setBounds(x, y, width, height);
}
