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
    auto height = (float)label.getHeight();
    return UXPalette::robotoLightItalic.withHeight(height);
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
