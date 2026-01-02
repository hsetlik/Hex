/*
  ==============================================================================

    ModulationGrid.h
    Created: 7 Jun 2021 2:24:59pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "SymbolButton.h"
#include "Audio/FMOperator.h"

class ModulationToggle : public juce::Button {
private:
  pButtonAttach attach;
  const int srcID;
  const int destID;

public:
  ModulationToggle(apvts* tree, int source, int dest);
  void paintButton(juce::Graphics& g, bool highlighted, bool down) override;
};

//=========================================================================
class ModulationGrid : public juce::Component {
public:
  ModulationGrid(apvts* tree);
  apvts* const linkedTree;
  void resized() override;
  void paint(juce::Graphics& g) override;

private:
  juce::OwnedArray<ModulationToggle> buttons;
};
