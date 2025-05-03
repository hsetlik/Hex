/*
  ==============================================================================

    SymbolButton.h
    Created: 4 Jun 2021 1:25:06pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"

using pButtonAttach = std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>;
using buttonAttach = juce::AudioProcessorValueTreeState::ButtonAttachment;


class SymbolButton : public juce::ShapeButton
{//! nice utility for making shape buttons with simple symbols
public:
    SymbolButton (juce::String name="symbol_button", juce::Colour bkgnd= UXPalette::lightGray, juce::Colour sColour= UXPalette::highlight) :
    juce::ShapeButton (name, bkgnd, bkgnd, bkgnd),
    upBkgnd (bkgnd),
    downBkgnd (bkgnd.darker (0.3f)),
    symbolOnColor (sColour),
    symbolOffColor (sColour),
    strokeMode (false),
    strokeWidth (1.0f),
    isRounded (false),
    cornerSize (1.0f),
    cornerFactor (6)
    {
        resized();
    }
    virtual ~SymbolButton() {}
    void paintButton (juce::Graphics& g, bool highlighted, bool down) override
    {
        if (down)
            g.setColour (downBkgnd);
        else
            g.setColour (upBkgnd);
        g.fillPath (bkgnd);
        if (getToggleState())
            g.setColour (symbolOnColor);
        else
            g.setColour (symbolOffColor);
        if (!strokeMode)
            g.fillPath (symbol);
        else
        {
            auto stroke = juce::PathStrokeType (strokeWidth);
            g.strokePath (symbol, stroke);
        }
    }
    void resized() override
    {
        auto fBounds = getLocalBounds().toFloat();
        cornerSize = fBounds.getWidth() / (float)cornerFactor;
        bkgnd.clear();
        if (isRounded)
            bkgnd.addRoundedRectangle (fBounds, cornerSize);
        else
            bkgnd.addRectangle (fBounds);
        symbol.clear();
        setSymbol();
    }
    void setRounded (bool shouldBeRounded, int corner=6)
    {
        isRounded = shouldBeRounded;
        cornerFactor = corner;
        resized();
    }
    void setStroke (bool shouldStroke, float width=1.0f)
    {
        strokeMode = shouldStroke;
        strokeWidth = width;
    }
    void setSymbolOnColor (Color col) {symbolOnColor = col; }
    void setSymbolOffColor (Color col) {symbolOffColor = col; }
    virtual void setSymbol() {}
protected:
    juce::Path symbol;
private:
    juce::Path bkgnd;
    juce::Colour upBkgnd;
    juce::Colour downBkgnd;
    juce::Colour symbolOnColor;
    juce::Colour symbolOffColor;
    bool strokeMode;
    float strokeWidth;
    bool isRounded;
    float cornerSize;
    int cornerFactor;
};
//===================================================================
class ModulationToggle : public juce::ShapeButton
{
public:
    ModulationToggle (int src, int dst);
    const int source;
    const int dest;
    void paintButton (juce::Graphics& g, bool highlighted, bool down) override;
private:
    Color litBkgndUpper;
    Color unlitBkgndUpper;
    Color litBkgndLower;
    Color unlitBkgndLower;
    Color litText;
    Color unlitText;
};

class OutputButton : public juce::ShapeButton
{
public:
    OutputButton();
    void paintButton (juce::Graphics& g, bool highlighted, bool down) override;
private:
    Color litBkgnd;
    Color unlitBkgnd;
    Color litText;
    Color unlitText;
};

class ArrowButton : public juce::ShapeButton
{
public:
    ArrowButton (juce::String name="arrow_button", juce::Colour bkgnd= UXPalette::lightGray, juce::Colour sColour= UXPalette::highlight);
    virtual juce::Path symbolPath()=0;
    void paintButton (juce::Graphics& g, bool highlighted, bool down) override;
private:
    Color bkgndColor;
    Color symbolColor;
};

class LeftButton : public ArrowButton
{
public:
    juce::Path symbolPath() override;
};

class RightButton : public ArrowButton
{
public:
    juce::Path symbolPath() override;
};

