/*
  ==============================================================================

    SymbolButton.cpp
    Created: 4 Jun 2021 1:25:06pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SymbolButton.h"
ModulationToggle::ModulationToggle(int src, int dst) :
juce::ShapeButton("name", UXPalette::darkGray, UXPalette::darkGray, UXPalette::darkGray),
source(src),
dest(dst),
litBkgndUpper(UXPalette::lightRed),
unlitBkgndUpper(UXPalette::darkRed),
litBkgndLower(UXPalette::lightBlue),
unlitBkgndLower(UXPalette::darkBlue),
litText(juce::Colours::white),
unlitText(UXPalette::darkGray)
{
    setClickingTogglesState(true);
}
void ModulationToggle::paintButton(juce::Graphics &g, bool highlighted, bool down)
{
    auto fBounds = getLocalBounds().toFloat();
    auto bBounds = fBounds.reduced(fBounds.getWidth() / 10.0f);
    auto bColor = (getToggleState())? litBkgndUpper : unlitBkgndUpper;
    g.setColour(bColor);
    g.fillRect(bBounds);
    juce::Path lowerBounds;
    lowerBounds.startNewSubPath(bBounds.getRight(), bBounds.getY());
    lowerBounds.lineTo(bBounds.getRight(), bBounds.getBottom());
    lowerBounds.lineTo(bBounds.getX(), bBounds.getBottom());
    lowerBounds.closeSubPath();
    auto lColor = (getToggleState())? litBkgndLower : unlitBkgndLower;
    g.setColour(lColor);
    g.fillPath(lowerBounds);
    auto sStr = juce::String(source + 1);
    auto dStr = juce::String(dest + 1);
    auto tColor = (getToggleState())? litText : unlitText;
    g.setColour(tColor);
    juce::Path slash;
    slash.startNewSubPath(bBounds.getX(), bBounds.getBottom());
    slash.lineTo(bBounds.getRight(), bBounds.getY());
    auto stroke = juce::PathStrokeType(1.0f);
    g.strokePath(slash, stroke);
    auto cushion = bBounds.getHeight() / 15.0f;
    bBounds = bBounds.reduced(cushion);
    auto dY = bBounds.getHeight() / 2.0f;
    auto font = g.getCurrentFont();
    auto fontHeight = font.getHeight() * 0.8f;
    g.setFont(font.withHeight(fontHeight));
    auto sBounds = bBounds.removeFromTop(dY);
    g.drawText(sStr, sBounds, juce::Justification::topLeft);
    g.drawText(dStr, bBounds, juce::Justification::bottomRight);
}
//===================================================================
OutputButton::OutputButton() :
juce::ShapeButton("outputToggle", UXPalette::darkGray, UXPalette::darkGray, UXPalette::darkGray),
litBkgnd(UXPalette::lightRed),
unlitBkgnd(UXPalette::darkRed),
litText(UXPalette::lightGray),
unlitText(UXPalette::darkGray)
{
    setClickingTogglesState(true);
}

void OutputButton::paintButton(juce::Graphics &g, bool highlighted, bool down)
{
    auto fBounds = getLocalBounds().toFloat();
    auto bColor = (getToggleState())? litBkgnd : unlitBkgnd;
    auto tColor = (getToggleState())? litText : unlitText;
    auto corner = fBounds.getHeight() / 6.0f;
    g.setColour(bColor);
    g.fillRoundedRectangle(fBounds, corner);
    auto tBounds = fBounds.reduced(corner * 1.5f);
    g.setColour(tColor);
    g.drawText("Output", tBounds, juce::Justification::centred);
}
