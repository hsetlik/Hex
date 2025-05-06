/*
  ==============================================================================

    HexEditor.h
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "BitmapWaveGraph.h"
#include "CustomLnF.h"
#include "OperatorComponent.h"
#include "ModulationGrid.h"
#include "PatchBrowser.h"
#include "LfoComponent.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_utils/juce_audio_utils.h"

class FilterPanel : public Component {
public:
  FilterPanel(apvts* tree, GraphParamSet* params);
  ~FilterPanel() override;
  apvts* const linkedTree;
  void resized() override;
  void paint(juce::Graphics& g) override;

private:
  EnvelopeComponent envComp;
  juce::Slider cutoffSlider;
  juce::Slider resSlider;
  juce::Slider wetSlider;
  juce::Slider depthSlider;

  RotaryParamName cutoffName;
  RotaryParamName resName;
  RotaryParamName wetName;
  RotaryParamName depthName;

  juce::ComboBox typeBox;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      typeAttach;

  pSliderAttach cutoffAttach;
  pSliderAttach resAttach;
  pSliderAttach wetAttach;
  pSliderAttach depthAttach;
  HexLookAndFeel lnf;
};
//========================================================
class BottomBar : public Component {
private:
  RotaryParamName velName;
  juce::Slider velSlider;
  pSliderAttach velAttach;
  juce::MidiKeyboardComponent kbdComp;

public:
  BottomBar(apvts* tree, juce::MidiKeyboardState& kbdState);
  void resized() override;
  void paint(juce::Graphics& g) override;
};
//========================================================
class HexEditor : public Component {
public:
  HexEditor(HexAudioProcessor* proc,
            apvts* tree,
            GraphParamSet* params,
            RingBuffer<float>* buffer,
            juce::MidiKeyboardState& kbdState);
  ~HexEditor() override;
  apvts* const linkedTree;
  void resized() override;
  void paint(juce::Graphics& g) override;

private:
  HexLookAndFeel lnf;
  juce::OwnedArray<OperatorComponent> opComponents;
  juce::OwnedArray<LfoComponent> lfoComponents;
  ModulationGrid modGrid;
  BitmapWaveGraph graph;
  FilterPanel fPanel;
  PatchLoader loader;
  PatchDialogBox saveDialog;
  BottomBar kbdBar;
};
