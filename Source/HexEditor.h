/*
  ==============================================================================

    HexEditor.h
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "OperatorComponent.h"
#include "ModulationGrid.h"

class HexEditor : public juce::Component
{
public:
    HexEditor(apvts* tree);
    apvts* const linkedTree;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::OwnedArray<OperatorComponent> opComponents;
    ModulationGrid modGrid;
};



