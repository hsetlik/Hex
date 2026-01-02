/*
  ==============================================================================

    SymbolButton.cpp
    Created: 4 Jun 2021 1:25:06pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/SymbolButton.h"
#include "juce_core/juce_core.h"

//===================================================================
OutputButton::OutputButton()
    : juce::ShapeButton("outputToggle",
                        UXPalette::darkGray,
                        UXPalette::darkGray,
                        UXPalette::darkGray),
      litBkgnd(UXPalette::lightRed),
      unlitBkgnd(UXPalette::darkRed),
      litText(juce::Colours::white),
      unlitText(UXPalette::darkGray) {
  setClickingTogglesState(true);
}

void OutputButton::paintButton(juce::Graphics& g, bool highlighted, bool down) {
  juce::ignoreUnused(highlighted, down);
  auto fBounds = getLocalBounds().toFloat();
  auto bColor = (getToggleState()) ? litBkgnd : unlitBkgnd;
  auto tColor = (getToggleState()) ? litText : unlitText;
  auto corner = fBounds.getHeight() / 6.0f;
  g.setColour(bColor);
  g.fillRoundedRectangle(fBounds, corner);
  auto tBounds = fBounds.reduced(corner * 1.5f);
  g.setColour(tColor);
  g.drawText("OUTPUT", tBounds, juce::Justification::centred);
}

//===================================================================

ArrowButton::ArrowButton(juce::String name,
                         juce::Colour bkgnd,
                         juce::Colour sColour)
    : juce::ShapeButton("arrow_button",
                        UXPalette::darkGray,
                        UXPalette::darkGray,
                        UXPalette::darkGray),
      bkgndColor(bkgnd),
      symbolColorActive(sColour) {
  symbolColorInactive = symbolColorActive.darker(0.4f);
  juce::ignoreUnused(name);
}

void ArrowButton::paintButton(juce::Graphics& g, bool highlighted, bool down) {
  juce::ignoreUnused(highlighted, down);
  auto bounds = getLocalBounds();
  auto corner = bounds.getHeight() / 8;
  g.setColour(bkgndColor);
  g.fillRoundedRectangle(bounds.toFloat(), (float)corner);
  auto fillColor = isEnabled() ? symbolColorActive : symbolColorInactive;
  g.setColour(fillColor);
  auto path = symbolPath();
  g.fillPath(path);
}

juce::Path LeftButton::symbolPath() {
  auto fullBounds = getLocalBounds().toFloat();
  auto dX = fullBounds.getWidth() / 4.0f;
  auto triBounds = fullBounds.reduced(dX);
  juce::Path path;
  path.startNewSubPath(triBounds.getRight(), triBounds.getY());
  path.lineTo(triBounds.getRight(), triBounds.getBottom());
  path.lineTo(triBounds.getX(), triBounds.getY() + (triBounds.getHeight() / 2));
  path.closeSubPath();
  return path;
}

juce::Path RightButton::symbolPath() {
  auto fullBounds = getLocalBounds().toFloat();
  auto dX = fullBounds.getWidth() / 4.0f;
  auto triBounds = fullBounds.reduced(dX);
  juce::Path path;
  path.startNewSubPath(triBounds.getX(), triBounds.getY());
  path.lineTo(triBounds.getX(), triBounds.getBottom());
  path.lineTo(triBounds.getRight(),
              triBounds.getY() + (triBounds.getHeight() / 2));
  path.closeSubPath();
  return path;
}
