/*
  ==============================================================================

    CustomLnF.cpp
    Created: 23 Jun 2021 12:05:42am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/CustomLnF.h"
#include "GUI/Color.h"
#include "juce_core/juce_core.h"
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
  return UXPalette::robotoLightItalic.withHeight(height);
}
void HexLookAndFeel::positionComboBoxText(juce::ComboBox& box,
                                          juce::Label& label) {
  label.setBounds(1, 1, box.getWidth() - 30, box.getHeight() - 2);
  label.setFont(getComboBoxFont(box));
}
//===========================================================================================
juce::Font HexLookAndFeel::getLabelFont(juce::Label& label) {
  juce::ignoreUnused(label);
  return UXPalette::robotoLightItalic.withHeight(10.0f);
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
  auto iBounds = juce::Rectangle<int>{x, y, width, height};
  auto fBounds = iBounds.toFloat();
  g.setColour(UXPalette::darkGray);
  g.fillEllipse(fBounds);
  g.setColour(UXPalette::lightGray);
  auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPosProportional;
  auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
  auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
  auto radius = fBounds.getWidth() * 0.4f;
  auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved,
                                         juce::PathStrokeType::rounded);
  juce::Path track;
  track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle,
                      rotaryEndAngle, true);
  g.strokePath(track, strokeType);
  g.setColour(UXPalette::highlight);
  auto iRadius = radius * 0.6f;
  juce::Path thumb;
  thumb.startNewSubPath(centerX, centerY - radius);
  thumb.lineTo(centerX, centerY - iRadius);
  thumb.closeSubPath();
  thumb.applyTransform(juce::AffineTransform::rotation(rotaryStartAngle + angle,
                                                       centerX, centerY));
  g.strokePath(thumb, strokeType);
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
  auto bkgndWidth = fBounds.getWidth() * 0.2f;
  auto xOffsetBkgnd = (fBounds.getWidth() / 2.0f) - (bkgndWidth / 2.0f);
  auto corner = bkgndWidth / 2.0f;
  auto thumbWidth = fBounds.getWidth() * 0.45f;

  auto thumbXOffset = (fBounds.getWidth() / 2.0f) - (thumbWidth / 2.0f);
  auto thumbHeight =
      (fBounds.getHeight() - (float)slider.getTextBoxHeight()) * 0.1f;
  auto thumbY = (1.0f - fPos) * (fBounds.getHeight() - thumbHeight) - 5;

  // draw the background
  g.setColour(UXPalette::darkGray);
  g.fillRoundedRectangle(
      (float)x + xOffsetBkgnd, 5, bkgndWidth,
      fBounds.getHeight() - ((float)slider.getTextBoxHeight() * 1.2f) - 5,
      corner);
  // draw the thumb
  g.setColour(UXPalette::highlight);
  g.fillRoundedRectangle((float)x + thumbXOffset, thumbY, thumbWidth,
                         thumbHeight, thumbHeight / 2.0f);
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
  return UXPalette::robotoLightItalic.withHeight((float)height * 0.8f);
}
