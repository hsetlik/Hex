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
litBkgnd(UXPalette::lightRed),
unlitBkgnd(UXPalette::darkRed),
litText(UXPalette::lightGray),
unlitText(UXPalette::darkGray)
{
    setClickingTogglesState(true);
}
void ModulationToggle::paintButton(juce::Graphics &g, bool highlighted, bool down)
{
    auto fBounds = getLocalBounds().toFloat();
    auto bBounds = fBounds.reduced(fBounds.getWidth() / 8.0f);
    auto bColor = (getToggleState())? litBkgnd : unlitBkgnd;
    g.setColour(bColor);
    g.fillRect(bBounds);
    auto sStr = juce::String(source);
    auto dStr = juce::String(dest);
    auto tColor = (getToggleState())? litText : unlitText;
    g.setColour(tColor);
    juce::Path slash;
    slash.startNewSubPath(bBounds.getX(), bBounds.getBottom());
    slash.lineTo(bBounds.getRight(), bBounds.getY());
    auto stroke = juce::PathStrokeType(1.0f);
    g.strokePath(slash, stroke);
    auto dY = bBounds.getHeight() / 2.0f;
    auto sBounds = bBounds.removeFromTop(dY);
    g.drawText(sStr, sBounds, juce::Justification::left);
    g.drawText(dStr, bBounds, juce::Justification::right);
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
