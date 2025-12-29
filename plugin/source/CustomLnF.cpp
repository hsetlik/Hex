/*
  ==============================================================================

    CustomLnF.cpp
    Created: 23 Jun 2021 12:05:42am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/CustomLnF.h"
#include "GUI/Assets.h"
#include "GUI/Color.h"
#include "GUI/Fonts.h"
#include "Identifiers.h"
#include "MathUtil.h"
#include "juce_core/juce_core.h"

constexpr float KnobTopAspectRatio = 24.0f / 35.0f;
void HexLookAndFeel::drawComboBox(juce::Graphics& g,
                                  int width,
                                  int height,
                                  bool isButtonDown,
                                  int buttonX,
                                  int buttonY,
                                  int buttonW,
                                  int buttonH,
                                  juce::ComboBox& box) {
  juce::ignoreUnused(box, isButtonDown);
  auto stroke = juce::PathStrokeType(2.5f);
  auto boxBounds = juce::Rectangle<int>(0, 0, width, height);
  auto corner = (float)height / 8.0f;
  g.setColour(UXPalette::lightGray);
  g.fillRoundedRectangle(boxBounds.toFloat(), corner);
  g.setColour(UXPalette::darkGray);
  boxBounds = boxBounds.reduced((int)corner);
  corner = (float)boxBounds.getHeight() / 8.0f;
  g.fillRoundedRectangle(boxBounds.toFloat(), corner);
  g.setColour(UXPalette::lightGray);
  juce::Path button;
  auto buttonBounds = juce::Rectangle<float>((float)buttonX, (float)buttonY,
                                             (float)buttonW, (float)buttonH);
  buttonBounds = buttonBounds.reduced((float)buttonW / 3.5f);
  button.startNewSubPath(buttonBounds.getX(), buttonBounds.getY());
  button.lineTo(buttonBounds.getRight(), buttonBounds.getY());
  button.lineTo(buttonBounds.getCentreX(), buttonBounds.getBottom());
  button.closeSubPath();

  g.fillPath(button);
}
juce::Label* HexLookAndFeel::createComboBoxTextBox(juce::ComboBox& box) {
  juce::ignoreUnused(box);
  return new juce::Label(juce::String(), juce::String());
}
juce::Font HexLookAndFeel::getComboBoxFont(juce::ComboBox& box) {
  auto height = (float)box.getHeight() * 0.35f;
  return Fonts::getFont(Fonts::RobotoLightItalic, height);
}
void HexLookAndFeel::positionComboBoxText(juce::ComboBox& box,
                                          juce::Label& label) {
  label.setBounds(1, 1, box.getWidth() - 30, box.getHeight() - 2);
  label.setFont(getComboBoxFont(box));
}
//===========================================================================================
juce::Font HexLookAndFeel::getLabelFont(juce::Label& label) {
  juce::ignoreUnused(label);
  return Fonts::getFont(Fonts::RobotoLightItalic, 10.0f);
}

void HexLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
  const juce::Font font(getLabelFont(label));
  g.setFont(font);
  g.setColour(juce::Colours::white);
  auto textArea =
      getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
  g.drawFittedText(label.getText(), textArea, label.getJustificationType(), 1,
                   label.getMinimumHorizontalScale());
}
//===========================================================================================
void HexLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                      int x,
                                      int y,
                                      int width,
                                      int height,
                                      float sliderPosProportional,
                                      float rotaryStartAngle,
                                      float rotaryEndAngle,
                                      juce::Slider& slider) {
  juce::ignoreUnused(slider);
  if (width != height) {
    if (width < height)
      height = width;
    else
      width = height;
  }
  irect_t iBounds(x, y, width, height);
  auto sideLength = std::min(width, height);
  iBounds = iBounds.withSizeKeepingCentre(sideLength, sideLength);
  auto bounds = iBounds.toFloat();
  // 1. draw the background image
  auto bkgndImg = Assets::getImage(Assets::KnobBkgnd);
  g.drawImage(bkgndImg, bounds);
  // 2. find the bounds for the knob top
  auto topBounds =
      bounds.withSizeKeepingCentre(bounds.getWidth() * KnobTopAspectRatio,
                                   bounds.getHeight() * KnobTopAspectRatio);
  auto topImg = Assets::getImage(Assets::KnobTop);
  auto endAngle =
      MathUtil::fLerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  auto transform = juce::AffineTransform::rotation(
      endAngle, topBounds.getCentreX(), topBounds.getCentreY());
  g.addTransform(transform);
  g.drawImage(topImg, topBounds);
}

void HexLookAndFeel::drawLinearSlider(juce::Graphics& g,
                                      int x,
                                      int y,
                                      int width,
                                      int height,
                                      float sliderPos,
                                      float minSliderPos,
                                      float maxSliderPos,
                                      const juce::Slider::SliderStyle style,
                                      juce::Slider& slider) {
  juce::ignoreUnused(y, width, height, minSliderPos, style);
  auto fPos = 1.0f - (sliderPos / maxSliderPos);
  auto fBounds = slider.getBounds().toFloat();
  const float xScale = fBounds.getWidth() / 28.0f;
  const float yScale = fBounds.getHeight() / 130.0f;
  const float trackWidth = 3.0f * xScale;
  const float trackX = (float)x + ((float)width / 2.0f) - (trackWidth / 2.0f);
  frect_t trackBounds = {trackX, (float)y, trackWidth, (float)height};
  g.setColour(UIColor::bkgndGray);
  g.fillRoundedRectangle(trackBounds, trackWidth / 2.0f);
  const float trackStroke = 1.0f * xScale;
  g.setColour(UIColor::borderGray);
  g.drawRoundedRectangle(trackBounds, trackWidth / 2.0f, trackStroke);
  const float thumbHeight = 20.0f * yScale;
  const float thumbMax = (float)y + (float)height - (thumbHeight / 2.0f);
  const float thumbMin = thumbHeight / 2.0f;
  const float thumbY = MathUtil::fLerp(thumbMax, thumbMin, fPos);
  frect_t thumbBounds = {(float)x, thumbY, (float)width, thumbHeight};
  auto& thumbImg = Assets::getImage(Assets::Thumb);
  g.drawImage(thumbImg, thumbBounds);

  // irect_t bounds = {x, y, width, height};
  // g.setColour(UIColor::greenLight);
  // g.fillRect(bounds);
}
//======================================================================================

void HexLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                          juce::Button& b,
                                          const juce::Colour& bColor,
                                          bool isHighlighted,
                                          bool isDown) {
  juce::ignoreUnused(bColor, isHighlighted, isDown);
  auto bBounds = b.getLocalBounds();
  auto corner = bBounds.getHeight() / 8;
  g.setColour(UXPalette::lightGray);
  g.fillRoundedRectangle(bBounds.toFloat(), (float)corner);
  bBounds = bBounds.reduced(corner);
  corner = bBounds.getHeight() / 8;
  g.setColour(UXPalette::darkGray);
  g.fillRoundedRectangle(bBounds.toFloat(), (float)corner);
}

void HexLookAndFeel::drawButtonText(juce::Graphics& g,
                                    juce::TextButton& b,
                                    bool isHighlighted,
                                    bool isDown) {
  juce::ignoreUnused(isHighlighted, isDown);
  auto fBounds = b.getLocalBounds().toFloat();
  auto textHeight = (int)(fBounds.getHeight() * 0.8f);
  auto font = getTextButtonFont(b, textHeight);
  g.setColour(juce::Colours::white);
  g.setFont(font);
  auto iBounds = b.getLocalBounds();
  auto iX = iBounds.getX();
  auto iY = iBounds.getY();
  auto iWidth = iBounds.getWidth();
  auto dX = iWidth / 11;
  auto textBox = juce::Rectangle<int>(iX + dX, iY, 8 * dX, iBounds.getHeight());
  g.drawFittedText(b.getButtonText(), textBox, juce::Justification::centred, 1);
}

int HexLookAndFeel::getTextButtonWidthToFitText(juce::TextButton& b,
                                                int height) {
  auto font = getTextButtonFont(b, height);
  auto str = b.getButtonText();
  auto fBounds = b.getLocalBounds().toFloat();
  juce::AttributedString aStr(str);
  aStr.setFont(font);
  juce::TextLayout layout;
  layout.createLayout(aStr, fBounds.getWidth());
  auto textWidth = layout.getWidth();
  auto dX = (float)textWidth / 8.0f;
  return (int)(10 * dX);
}

juce::Font HexLookAndFeel::getTextButtonFont(juce::TextButton& b, int height) {
  juce::ignoreUnused(b);
  return Fonts::getFont(Fonts::RobotoLightItalic, (float)height * 0.85f);
}
