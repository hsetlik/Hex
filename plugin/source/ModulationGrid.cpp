/*
  ==============================================================================

    ModulationGrid.cpp
    Created: 7 Jun 2021 2:24:59pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/ModulationGrid.h"
#include "GUI/Color.h"
#include "Identifiers.h"
ModulationGrid::ModulationGrid(apvts* tree) : linkedTree(tree) {
  for (int src = 0; src < NUM_OPERATORS; ++src) {
    for (int dst = 0; dst < NUM_OPERATORS; ++dst) {
      addAndMakeVisible(buttons.add(new ModulationToggle(src, dst)));
      size_t idx = (size_t)(src * NUM_OPERATORS) + (size_t)dst;
      auto str = juce::String(src) + "to" + juce::String(dst) + "Param";
      auto btn = buttons.getLast();
      attachments[idx].reset(new buttonAttach(*linkedTree, str, *btn));
    }
  }
}

void ModulationGrid::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto innerBounds = fBounds.reduced(fBounds.getWidth() / 8.0f);
  auto x0 = innerBounds.getX();
  auto y0 = innerBounds.getY();
  auto dX = innerBounds.getWidth() / NUM_OPERATORS;
  auto dY = innerBounds.getWidth() / NUM_OPERATORS;
  for (int src = 0; src < NUM_OPERATORS; ++src) {
    for (int dst = 0; dst < NUM_OPERATORS; ++dst) {
      frect_t fBox = {x0 + (float)src * dX, y0 + (float)dst * dY, dX, dY};
      auto idx = (src * NUM_OPERATORS) + dst;
      buttons[idx]->setBounds(fBox.toNearestInt());
    }
  }
}

void ModulationGrid::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  const float barWidth = fBounds.getWidth() / 8.0f;
  auto topBounds = fBounds.removeFromTop(barWidth);
  topBounds.removeFromLeft(barWidth);
  AttString topStr("Modulator");
  auto font = UXPalette::robotoBlackItalic.withHeight(barWidth * 0.9f);
  topStr.setFont(font);
  topStr.setColour(UXPalette::darkRed);
  topStr.setJustification(juce::Justification::centred);
  topStr.draw(g, topBounds);

  juce::GlyphArrangement ga;
  ga.addFittedText(font, "Carrier", topBounds.getX(), topBounds.getY(),
                   topBounds.getWidth(), topBounds.getHeight(),
                   juce::Justification::centredLeft, 1);
  juce::Path p;
  ga.createPath(p);
  auto pBounds = p.getBounds().toFloat();

  auto transform = juce::AffineTransform()
                       .rotated(-juce::MathConstants<float>::halfPi,
                                pBounds.getX(), pBounds.getBottom())
                       .translated(-(barWidth / 6.0f),
                                   pBounds.getWidth() + (barWidth * 2.0f));
  p.applyTransform(transform);
  g.setColour(UXPalette::lightBlue);
  g.fillPath(p);
}
