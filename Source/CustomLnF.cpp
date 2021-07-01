/*
  ==============================================================================

    CustomLnF.cpp
    Created: 23 Jun 2021 12:05:42am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "CustomLnF.h"

juce::Font HexLookAndFeel::getLabelFont(juce::Label &label)
{
    return UXPalette::robotoLightItalic.withHeight(10.0f);
}

void HexLookAndFeel::drawLabel(juce::Graphics &g, juce::Label &label)
{
    const juce::Font font(getLabelFont(label));
    g.setFont (font);
    g.setColour(juce::Colours::white);
    auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
    g.drawFittedText (label.getText(),
                      textArea,
                      label.getJustificationType(),
                      1,
                      label.getMinimumHorizontalScale());
}
void HexLookAndFeel::drawRotarySlider(juce::Graphics &g,
                                      int x, int y, int width, int height,
                                      float sliderPosProportional,
                                      float rotaryStartAngle,
                                      float rotaryEndAngle,
                                      juce::Slider &slider)
{
    auto iBounds = juce::Rectangle<int> {x, y, width, height};
    auto fBounds = iBounds.toFloat();
    g.setColour(UXPalette::darkGray);
    g.fillEllipse(fBounds);
    g.setColour(UXPalette::lightGray);
    auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPosProportional;
    auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
    auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
    auto radius = fBounds.getWidth() * 0.4f;
    auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(track, strokeType);
    g.setColour(UXPalette::highlight);
    auto iRadius = radius * 0.6f;
    juce::Path thumb;
    thumb.startNewSubPath(centerX, centerY - radius);
    thumb.lineTo(centerX, centerY - iRadius);
    thumb.closeSubPath();
    thumb.applyTransform(juce::AffineTransform::rotation(rotaryStartAngle + angle, centerX, centerY));
    g.strokePath(thumb, strokeType);
}
void HexLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider &slider)
{
    auto fPos = 1.0f - (sliderPos / maxSliderPos);
    auto fBounds = slider.getBounds().toFloat();
    auto bkgndWidth = fBounds.getWidth() * 0.2f;
    auto xOffsetBkgnd = (fBounds.getWidth() / 2.0f) - (bkgndWidth / 2.0f);
    auto corner = bkgndWidth / 2.0f;
    auto thumbWidth = fBounds.getWidth() * 0.45f;
    
    auto thumbXOffset = (fBounds.getWidth() / 2.0f) - (thumbWidth / 2.0f);
    auto thumbHeight = (fBounds.getHeight() - slider.getTextBoxHeight()) * 0.1f;
    auto thumbY = (1.0f - fPos) * (fBounds.getHeight() - thumbHeight) - 5;
    
    //draw the background
    g.setColour(UXPalette::darkGray);
    g.fillRoundedRectangle(x + xOffsetBkgnd,
                           5,
                           bkgndWidth,
                           fBounds.getHeight() - (slider.getTextBoxHeight() * 1.2f) - 5,
                           corner);
    //draw the thumb
    g.setColour(UXPalette::highlight);
    g.fillRoundedRectangle(x + thumbXOffset,
                           thumbY,
                           thumbWidth,
                           thumbHeight,
                           thumbHeight / 2.0f);
    
}
