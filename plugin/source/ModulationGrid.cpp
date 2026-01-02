/*
  ==============================================================================

    ModulationGrid.cpp
    Created: 7 Jun 2021 2:24:59pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/ModulationGrid.h"
#include "GUI/Color.h"
#include "GUI/Fonts.h"
#include "Identifiers.h"

ModulationToggle::ModulationToggle(apvts* tree, int source, int dest)
    : juce::Button("ModToggle"), srcID(source), destID(dest) {
  const String paramID = String(srcID) + "to" + String(destID) + "Param";
  setClickingTogglesState(true);
  setRepaintsOnMouseActivity(true);
  attach.reset(new buttonAttach(*tree, paramID, *this));
}

void ModulationToggle::paintButton(juce::Graphics& g,
                                   bool highlighted,
                                   bool down) {
  juce::ignoreUnused(highlighted, down);
  auto fBounds = getLocalBounds().toFloat();
  auto inset = fBounds.getWidth() / 10.0f;
  fBounds = fBounds.reduced(inset);
  g.setColour(UIColor::borderGray);
  g.fillRect(fBounds);
  const float dX = fBounds.getWidth() / 3.0f;
  frect_t srcBounds = {inset, inset, dX, fBounds.getHeight()};
  frect_t destBounds = {inset + (2.0f * dX), inset, dX, fBounds.getHeight()};
  auto font =
      Fonts::getFont(Fonts::KenyanReg).withHeight(fBounds.getHeight() * 0.5f);
  AttString srcStr(String(srcID + 1));
  srcStr.setFont(font);
  srcStr.setJustification(juce::Justification::centred);
  auto srcColor = getToggleState() ? UIColor::greenLight : UIColor::bkgndGray;
  srcStr.setColour(srcColor);
  srcStr.draw(g, srcBounds);

  AttString destStr(String(destID + 1));
  destStr.setFont(font);
  destStr.setJustification(juce::Justification::centred);
  auto destColor = getToggleState() ? UIColor::orangeLight : UIColor::bkgndGray;
  destStr.setColour(destColor);
  destStr.draw(g, destBounds);
}

//=========================================================================

ModulationGrid::ModulationGrid(apvts* tree) : linkedTree(tree) {
  for (int src = 0; src < NUM_OPERATORS; ++src) {
    for (int dst = 0; dst < NUM_OPERATORS; ++dst) {
      addAndMakeVisible(
          buttons.add(new ModulationToggle(linkedTree, src, dst)));
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
  auto font = Fonts::getFont(Fonts::RobotoBlackItalic, 0.9f * barWidth);
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
