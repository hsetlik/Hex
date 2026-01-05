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

//===============================================

static juce::Path getComboBoxButtonPath(int x,
                                        int y,
                                        int width,
                                        int height,
                                        bool isDown) {
  irect_t iBounds = {x, y, width, height};
  auto outerFBounds = iBounds.toFloat();
  auto btnWidth = outerFBounds.getWidth() * 0.4f;
  auto btnHeight = outerFBounds.getHeight() * 0.4f;
  auto fBounds = outerFBounds.withSizeKeepingCentre(btnWidth, btnWidth);
  juce::Path p;
  const float dX = btnWidth / 8.0f;
  const float dY = btnHeight / 8.0f;
  const float x0 = fBounds.getX();
  const float y0 = fBounds.getY();
  p.startNewSubPath(x0, y0);
  if (isDown) {
    p.lineTo(x0 + 4.0f * dX, y0 + 8.0f * dY);
    p.lineTo(x0 + 8.0f * dX, y0);
  } else {
    p.lineTo(x0 + 8.0f * dX, y0 + 4.0f * dY);
    p.lineTo(x0, y0 + 8.0f * dY);
  }
  p.closeSubPath();
  return p;
}

void HexLookAndFeel::drawComboBox(juce::Graphics& g,
                                  int width,
                                  int height,
                                  bool isButtonDown,
                                  int buttonX,
                                  int buttonY,
                                  int buttonW,
                                  int buttonH,
                                  juce::ComboBox& box) {
  juce::ignoreUnused(isButtonDown);
  const float yScale = (float)height / 20.0f;
  irect_t iBounds = {0, 0, width, height};
  auto fBounds = iBounds.toFloat();
  const float radius = 2.0f * yScale;
  const float strokeWeight = 0.25f * yScale;
  g.setColour(UIColor::bkgndGray);
  g.fillRoundedRectangle(fBounds, radius);
  g.setColour(UIColor::offWhite);
  g.drawRoundedRectangle(fBounds, radius, strokeWeight);
  auto btnPath = getComboBoxButtonPath(buttonX, buttonY, buttonW, buttonH,
                                       box.isPopupActive());
  g.fillPath(btnPath);
}

juce::Label* HexLookAndFeel::createComboBoxTextBox(juce::ComboBox& box) {
  juce::ignoreUnused(box);
  return new juce::Label(juce::String(), juce::String());
}

juce::Font HexLookAndFeel::getComboBoxFont(juce::ComboBox& box) {
  auto height = (float)box.getHeight() * 0.55f;
  return Fonts::getFont(Fonts::RobotoLightItalic, height);
}

void HexLookAndFeel::positionComboBoxText(juce::ComboBox& box,
                                          juce::Label& label) {
  auto fBounds = box.getLocalBounds().toFloat();
  const float yScale = fBounds.getHeight() / 20.0f;
  const float cushion = 3.0f * yScale;
  fBounds.removeFromLeft(cushion);
  fBounds.removeFromRight(yScale * 25.0f);
  label.setBounds(fBounds.toNearestInt());
  label.setFont(getComboBoxFont(box));
}
//===========================================================================================
void HexLookAndFeel::drawPopupMenuItem(juce::Graphics& g,
                                       const juce::Rectangle<int>& area,
                                       bool isSeparator,
                                       bool isActive,
                                       bool isHighlighted,
                                       bool isTicked,
                                       bool hasSubMenu,
                                       const String& text,
                                       const String& shortcutKeyText,
                                       const juce::Drawable* icon,
                                       const juce::Colour* textColour) {
  juce::ignoreUnused(isSeparator, isActive, isTicked, hasSubMenu,
                     shortcutKeyText, icon, textColour);
  auto fBounds = area.toFloat();
  g.setColour(UIColor::bkgndGray);
  g.fillRect(fBounds);
  const float yScale = fBounds.getHeight() / 20.0f;
  const float strokeWeight = 0.5f * yScale;
  auto borderColor = isHighlighted ? UIColor::offWhite : UIColor::borderGray;
  g.setColour(borderColor);
  g.drawRect(fBounds, strokeWeight);
  AttString aStr(text);
  fBounds.removeFromLeft(2.5f * yScale);
  auto font = isHighlighted ? Fonts::getFont(Fonts::RobotoMediumItalic)
                            : Fonts::getFont(Fonts::RobotoLightItalic);
  auto txtColor = isHighlighted ? UIColor::dulledWhite : UIColor::offWhite;
  aStr.setFont(font.withHeight(14.0f * yScale));
  aStr.setColour(txtColor);
  aStr.setJustification(juce::Justification::centredLeft);
  aStr.draw(g, fBounds);
}

void HexLookAndFeel::drawPopupMenuBackground(juce::Graphics& g,
                                             int width,
                                             int height) {
  irect_t bounds = {0, 0, width, height};
  g.setColour(UIColor::shadowGray);
  g.fillRect(bounds);
}

//===========================================================================================
juce::Font HexLookAndFeel::getLabelFont(juce::Label& label) {
  auto fBounds = label.getLocalBounds().toFloat();
  return Fonts::getFont(Fonts::RobotoLightItalic, fBounds.getHeight() * 0.55f);
}

void HexLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
  const juce::Font font(getLabelFont(label));
  auto fBounds = label.getLocalBounds().toFloat();
  AttString aStr(label.getText());

  aStr.setColour(UIColor::offWhite);
  aStr.setJustification(label.getJustificationType());
  aStr.setFont(font);
  aStr.setWordWrap(AttString::none);
  aStr.draw(g, fBounds.toFloat());
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
  juce::ignoreUnused(style, minSliderPos);
  auto fPos = 1.0f - (sliderPos / (maxSliderPos));
  auto fBounds = slider.getBounds().toFloat();
  const float xScale = fBounds.getWidth() / 28.0f;
  const float yScale = fBounds.getHeight() / 100.0f;
  const float trackWidth = 3.0f * xScale;
  const float trackX = (float)x + ((float)width / 2.0f) - (trackWidth / 2.0f);
  frect_t trackBounds = {trackX, (float)y, trackWidth, (float)height};
  g.setColour(UIColor::bkgndGray);
  g.fillRoundedRectangle(trackBounds, trackWidth / 2.0f);
  const float trackStroke = 1.0f * xScale;
  g.setColour(UIColor::borderGray);
  g.drawRoundedRectangle(trackBounds, trackWidth / 2.0f, trackStroke);
  const float thumbHeight = 20.0f * yScale;
  const float thumbMax = (float)y + (float)height;
  const float thumbMin = (float)y;
  const float thumbY =
      MathUtil::fLerp(thumbMax, thumbMin, fPos) - (thumbHeight * 0.5f);
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
  auto fBounds = b.getLocalBounds().toFloat();
  const float yScale = fBounds.getHeight() / 20.0f;
  const float radius = 2.0f * yScale;
  const float strokeWeight = 0.25f * yScale;
  g.setColour(UIColor::bkgndGray);
  g.fillRoundedRectangle(fBounds, radius);
  g.setColour(UIColor::offWhite);
  g.drawRoundedRectangle(fBounds, radius, strokeWeight);
}

void HexLookAndFeel::drawButtonText(juce::Graphics& g,
                                    juce::TextButton& b,
                                    bool isHighlighted,
                                    bool isDown) {
  juce::ignoreUnused(isHighlighted, isDown);
  auto fBounds = b.getLocalBounds().toFloat();
  auto textHeight = (int)(fBounds.getHeight());
  auto font = getTextButtonFont(b, textHeight);
  AttString aStr(b.getButtonText());
  aStr.setJustification(juce::Justification::centred);
  aStr.setFont(font);
  aStr.setColour(UIColor::offWhite);
  aStr.draw(g, fBounds);
}

int HexLookAndFeel::getTextButtonWidthToFitText(juce::TextButton& b,
                                                int height) {
  auto font = getTextButtonFont(b, height);
  auto str = b.getButtonText();
  juce::AttributedString aStr(str);
  aStr.setFont(font);
  auto fWidth = juce::TextLayout::getStringWidth(aStr);
  return (int)(fWidth * 1.08f);
}

juce::Font HexLookAndFeel::getTextButtonFont(juce::TextButton& b, int height) {
  juce::ignoreUnused(b);
  const float minHeight = std::min(18.0f, (float)height * 0.6f);
  return Fonts::getFont(Fonts::RobotoLightItalic, minHeight);
}
