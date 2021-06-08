/*
  ==============================================================================

    ModulationGrid.h
    Created: 7 Jun 2021 2:24:59pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "SymbolButton.h"
#include "FMOperator.h"


class ModulationGrid : public juce::Component
{
public:
    ModulationGrid(apvts* tree);
    apvts* const linkedTree;
    void resized() override;
private:
    juce::OwnedArray<ModulationToggle> buttons;
    std::array<pButtonAttach, NUM_OPERATORS * NUM_OPERATORS> attachments;
};
