/*
  ==============================================================================

    OperatorComponent.h
    Created: 7 Jun 2021 2:24:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "FMOperator.h"
#include "SymbolButton.h"
#include "ComponentUtil.h"
#define REPAINT_FPS 24
using sliderAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
using pSliderAttach = std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>;
class EnvelopeComponent : public juce::Component
{
public:
    class DAHDSRGraph :
    public juce::Component,
    public juce::Slider::Listener,
    public juce::Timer
    {
    public:
        DAHDSRGraph(EnvelopeComponent* parent);
        void sliderValueChanged(juce::Slider* slider) override;
        void paint(juce::Graphics& g) override;
        void timerCallback() override;
        juce::Slider* const pDelay;
        juce::Slider* const pAttack;
        juce::Slider* const pHold;
        juce::Slider* const pDecay;
        juce::Slider* const pSustain;
        juce::Slider* const pRelease;
        float delayVal;
        float attackVal;
        float holdVal;
        float decayVal;
        float sustainVal;
        float releaseVal;
        bool needsRepaint;
    };
    EnvelopeComponent(int idx, apvts* tree);
    const int opIndex;
    apvts* const linkedTree;
    void resized() override;
    
    DAHDSRGraph graph;
    
    juce::Slider delaySlider;
    juce::Slider attackSlider;
    juce::Slider holdSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    
    pSliderAttach delayAttach;
    pSliderAttach attackAttach;
    pSliderAttach holdAttach;
    pSliderAttach decayAttach;
    pSliderAttach sustainAttach;
    pSliderAttach releaseAttach;
};
//============================================================================
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
//===============================================================
class WaveSelector : public juce::Component
{//! TODO: this needs to be set up w a secret comboBox and listeners for apvts attachment
public:
    WaveSelector(int idx, apvts* tree);
    const int opIndex;
    apvts* const linkedTree;
    void resized() override;
private:
    SineButton bSine;
    SquareButton bSquare;
    SawButton bSaw;
    TriButton bTri;
};
//===============================================================
class OperatorComponent :
public juce::Component,
public juce::Button::Listener
{
public:
    OperatorComponent(int idx, apvts* tree);
    const int opIndex;
    apvts* const linkedTree;
    void resized() override;
    void buttonClicked(juce::Button* b) override;
private:
    EnvelopeComponent envComponent;
    WaveSelector waveSelect;
    OutputButton outButton;
    juce::Slider ratioSlider;
    juce::Slider modSlider;
    juce::Slider panSlider;
    
    pSliderAttach ratioAttach;
    pSliderAttach modAttach;
    pSliderAttach panAttach;
    
    pButtonAttach outAttach;
};
