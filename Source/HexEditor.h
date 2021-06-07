/*
  ==============================================================================

    HexEditor.h
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "SymbolButton.h"
#include "FMOperator.h"
//! button classes for wave selection
class WaveButton : public SymbolButton
{
public:
    WaveButton()
    {
        setClickingTogglesState(true);
        setStroke(true, 1.5f);
        setSymbolOnColor(UXPalette::highlight);
        setSymbolOffColor(UXPalette::darkGray);
    }
};
class SineButton : public WaveButton
{
public:
    void setSymbol() override;
};
class SquareButton : public WaveButton
{
public:
    void setSymbol() override;
};
class SawButton : public WaveButton
{
public:
    void setSymbol() override;
};
class TriButton : public WaveButton
{
public:
    void setSymbol() override;
};
//=======================================================


class WaveSelector : public juce::Component
{
public:
    WaveSelector(int opIndex, apvts* tree);
private:
    
};
