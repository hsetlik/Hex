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
#include "WaveGraphComponent.h"
#include "PatchBrowser.h"
#include "LfoComponent.h"

class FilterPanel : public juce::Component
{
public:
    FilterPanel(apvts* tree, GraphParamSet* params);
    apvts* const linkedTree;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    EnvelopeComponent envComp;
    juce::Slider cutoffSlider;
    juce::Slider resSlider;
    juce::Slider wetSlider;
    juce::Slider depthSlider;
    
    juce::ComboBox typeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttach;
    
    pSliderAttach cutoffAttach;
    pSliderAttach resAttach;
    pSliderAttach wetAttach;
    pSliderAttach depthAttach;
};

class HexEditor : public juce::Component
{
public:
    HexEditor(HexAudioProcessor* proc, apvts* tree, GraphParamSet* params, RingBuffer<GLfloat>* buffer);
    apvts* const linkedTree;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::OwnedArray<OperatorComponent> opComponents;
    juce::OwnedArray<LfoComponent> lfoComponents;
    ModulationGrid modGrid;
    WaveGraph graph;
    FilterPanel fPanel;
    PatchLoader loader;
    PatchDialogBox saveDialog;
};



