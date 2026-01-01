/*
  ==============================================================================

    OperatorComponent.h
    Created: 7 Jun 2021 2:24:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "Audio/Synthesizer.h"
#include "GUI/WaveformSelector.h"
#include "SymbolButton.h"
#include "SliderLabel.h"
#include "CustomLnF.h"
#include "EnvelopeComponent.h"
//===============================================================
class OutputToggle : public juce::Button {
public:
  OutputToggle();
  void paintButton(juce::Graphics& g, bool highlighted, bool down) override;
};
//===============================================================
class OperatorComponent : public juce::Component,
                          public juce::Button::Listener {
public:
  OperatorComponent(int idx, apvts* tree, GraphParamSet* gParams);
  ~OperatorComponent() override;
  const int opIndex;
  apvts* const linkedTree;
  void resized() override;
  void buttonClicked(juce::Button* b) override;
  void paint(juce::Graphics& g) override;

private:
  EnvelopeComponent envComponent;
  WaveformSelector waveSelect;
  OutputToggle outButton;
  juce::Slider ratioSlider;
  RotaryParamName ratioName;
  juce::Slider modSlider;
  RotaryParamName modName;
  juce::Slider panSlider;
  RotaryParamName panName;
  juce::Slider levelSlider;
  RotaryParamName levelName;

  RotaryLabel ratioLabel;
  RotaryLabel modLabel;
  RotaryLabel panLabel;
  RotaryLabel levelLabel;

  pSliderAttach ratioAttach;
  pSliderAttach modAttach;
  pSliderAttach panAttach;
  pSliderAttach levelAttach;

  pButtonAttach outAttach;
};
