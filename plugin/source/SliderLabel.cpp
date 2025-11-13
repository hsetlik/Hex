/*
  ==============================================================================

    SliderLabel.cpp
    Created: 7 Jun 2021 7:19:58pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SliderLabel.h"

RotaryLabel::RotaryLabel(juce::Slider* s, int decPlaces)
    : linkedSlider(s), decimalPlaces(decPlaces) {
  linkedSlider->addListener(this);
  setEditable(true);
  auto str = juce::String(linkedSlider->getValue());
  if (str.length() > decimalPlaces)
    str = str.substring(0, decimalPlaces);
  setText(str, juce::dontSendNotification);
}

void RotaryLabel::sliderValueChanged(juce::Slider* s) {
  jassert(s == linkedSlider);
  auto str = juce::String(linkedSlider->getValue());
  if (str.length() > decimalPlaces)
    str = str.substring(0, decimalPlaces);
  setText(str, juce::dontSendNotification);
}

juce::String RotaryLabel::currentValueString() {
  auto str = juce::String(linkedSlider->getValue());
  if (str.length() > decimalPlaces)
    str = str.substring(0, decimalPlaces);
  return str;
}

void RotaryLabel::componentMovedOrResized(juce::Component& component,
                                          bool wasMoved,
                                          bool wasResized) {
  juce::ignoreUnused(wasMoved, wasResized);
  auto& lf = getLookAndFeel();
  auto f = lf.getLabelFont(*this);
  auto borderSize = lf.getLabelBorderSize(*this);

  if (isAttachedOnLeft()) {
    int width = (int)fmin((int)(f.getStringWidthFloat(getText()) * 3.5f) +
                              borderSize.getLeftAndRight(),
                          component.getX());

    setBounds(component.getX() - width, component.getY(), width,
              component.getHeight());
  } else {
    auto height =
        borderSize.getTopAndBottom() + 2 + (int)(f.getHeight() * 1.5f);
    auto stringWidth = f.getStringWidthFloat(currentValueString());
    auto width = (int)(stringWidth) + borderSize.getLeftAndRight();
    auto centerX = component.getX() + (int)((float)component.getWidth() / 2.0f);
    auto x = centerX - (int)((float)width / 2.0f);
    setBounds(x, component.getBottom() + 1, width, height);
  }
}

void RotaryParamName::componentMovedOrResized(juce::Component& component,
                                              bool wasMoved,
                                              bool wasResized) {
  juce::ignoreUnused(wasMoved, wasResized);
  auto& lf = getLookAndFeel();
  auto f = lf.getLabelFont(*this);
  auto borderSize = lf.getLabelBorderSize(*this);

  if (isAttachedOnLeft()) {
    int width = (int)fmin((int)(f.getStringWidthFloat(text) * 3.5f) +
                              borderSize.getLeftAndRight(),
                          component.getX());

    setBounds(component.getX() - width, component.getY(), width,
              component.getHeight());
  } else {
    auto height =
        borderSize.getTopAndBottom() + 2 + (int)(f.getHeight() * 1.5f);
    auto width =
        (int)(f.getStringWidthFloat(text)) + borderSize.getLeftAndRight();
    auto dX = (component.getWidth() - width) / 2;
    auto gap = (int)((float)component.getHeight() * fLift);
    setBounds(component.getX() + dX, component.getY() - height - gap, width,
              height);
  }
}

void VerticalParamName::componentMovedOrResized(juce::Component& component,
                                                bool wasMoved,
                                                bool wasResized) {
  juce::ignoreUnused(wasMoved, wasResized);
  auto& lf = getLookAndFeel();
  auto f = lf.getLabelFont(*this);
  auto borderSize = lf.getLabelBorderSize(*this);

  if (isAttachedOnLeft()) {
    int width = (int)fmin((int)(f.getStringWidthFloat(text) * 3.5f) +
                              borderSize.getLeftAndRight(),
                          component.getX());

    setBounds(component.getX() - width, component.getY(), width,
              component.getHeight());
  } else {
    auto height =
        borderSize.getTopAndBottom() + 2 + (int)(f.getHeight() * 1.5f);
    auto width =
        (int)(f.getStringWidthFloat(text)) + borderSize.getLeftAndRight();
    auto xCenter = component.getX() + (component.getWidth() / 2);
    setBounds(xCenter - (width / 2), component.getBottom(), width, height);
  }
}
