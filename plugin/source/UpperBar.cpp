//===================================================
#include "GUI/UpperBar.h"
#include "GUI/Color.h"
#include "GUI/Fonts.h"
#include "HexHeader.h"

UpperBar::UpperBar(HexState* s, const String& verString)
    : versionString(verString), loader(s) {
  addAndMakeVisible(loader);
}

void UpperBar::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 1800.0f;
  const float yScale = fBounds.getHeight() / 100.0f;
  frect_t loaderBnds = {1411.0f * xScale, 40.0f * yScale, 358.0f * xScale, 22.0f * yScale};
  loader.setBounds(loaderBnds.toNearestInt());
}

void UpperBar::paint(juce::Graphics& g){
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 1800.0f;
  const float yScale = fBounds.getHeight() / 100.0f; 
  g.setColour(UIColor::bkgndGray);
  g.fillRect(fBounds);

  AttString hexStr("Hex");
  const float hexHeight = 79.8f * yScale;
  hexStr.setFont(Fonts::getFont(Fonts::KenyanBoldItalic).withHeight(hexHeight));
  hexStr.setColour(UIColor::orangeLight);
  hexStr.setJustification(juce::Justification::centredLeft);
  hexStr.setWordWrap(AttString::none);
  const float hexWidth = 99.0f * xScale;
  frect_t hexBnds = {28.0f * xScale, 6.7f * yScale, hexWidth, hexHeight};
  juce::TextLayout hexLayout;
  hexLayout.createLayout(hexStr, hexWidth);
  hexLayout.draw(g, hexBnds);
}
