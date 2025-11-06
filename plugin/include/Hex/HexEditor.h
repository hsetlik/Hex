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
#include "HexState.h"
#include "OperatorComponent.h"
#include "ModulationGrid.h"
#include "LfoComponent.h"
#include "PatchBrowser.h"
#include "PluginProcessor.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_utils/juce_audio_utils.h"

class FilterPanel : public Component {
public:
  FilterPanel(HexState* tree, GraphParamSet* params);
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

class HexEditor : public PatchBrowserParent {
public:
  HexEditor(HexAudioProcessor* proc,
            HexState* tree,
            GraphParamSet* params,
            RingBuffer<float>* buffer,
            juce::MidiKeyboardState& kbdState);
  ~HexEditor() override;
  apvts* const linkedTree;
  void resized() override;
  void paint(juce::Graphics& g) override;

  // patch browser overrides
  void openSaveDialog(const String& patchName) override;
  void openLoadDialog(const String& patchName) override;
  void closeModal() override;

private:
  HexLookAndFeel lnf;
  juce::OwnedArray<OperatorComponent> opComponents;
  juce::OwnedArray<LfoComponent> lfoComponents;
  ModulationGrid modGrid;
  BitmapWaveGraph graph;
  FilterPanel fPanel;
  BottomBar kbdBar;
  PatchLoader loader;
  // modal handling stuff---------------
  SaveDialog saveDialog;
  // hold on to a list of the components that we need to disable when a modal
  // component is active
  std::vector<Component*> nonModalComps;
  void setNonModalsEnabled(bool enabled);
};

//========================================================
