/*
  ==============================================================================

    OperatorComponent.h
    Created: 7 Jun 2021 2:24:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "Synthesizer.h"
#include "SymbolButton.h"
#include "ComponentUtil.h"
#include "SliderLabel.h"
#include "CustomLnF.h"
#define REPAINT_FPS 24
#define NOISE_SEED 2239
using sliderAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
using pSliderAttach = std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>;
using buttonAttach = juce::AudioProcessorValueTreeState::ButtonAttachment;
using pButtonAttach = std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>;
using comboBoxAttach = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using pComboBoxAttach = std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>;

class EnvelopeComponent : public juce::Component
{
public:
    class DAHDSRGraph :
    public juce::Component,
    public juce::Slider::Listener,
    public juce::Timer
    {
    public:
        DAHDSRGraph (EnvelopeComponent* parent);
        void sliderValueChanged (juce::Slider* slider) override;
        void paint (juce::Graphics& g) override;
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
    class LevelMeter :
    public juce::Component,
    public juce::AsyncUpdater,
    public juce::Timer
    {
    public:
        LevelMeter (int idx, GraphParamSet* params, bool filter);
        const int envIndex;
        const bool isFilter;
        GraphParamSet* const linkedParams;
        void timerCallback() override;
        void handleAsyncUpdate() override;
        void paint (juce::Graphics& g) override;
    private:
        float level;
        int lastVoice;
    };
    EnvelopeComponent (int idx, apvts* tree, GraphParamSet* gParams, bool isFilterComp = false);
    ~EnvelopeComponent();
    const int opIndex;
    const bool isFilter;
    apvts* const linkedTree;
    void resized() override;
    
    DAHDSRGraph graph;
    
    LevelMeter meter;
    
    juce::Slider delaySlider;
    juce::Slider attackSlider;
    juce::Slider holdSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    
    VerticalParamName delayName;
    VerticalParamName attackName;
    VerticalParamName holdName;
    VerticalParamName decayName;
    VerticalParamName sustainName;
    VerticalParamName releaseName;
    
    
    pSliderAttach delayAttach;
    pSliderAttach attackAttach;
    pSliderAttach holdAttach;
    pSliderAttach decayAttach;
    pSliderAttach sustainAttach;
    pSliderAttach releaseAttach;
    
    HexLookAndFeel lnf;
};

//============================================================================
//! button classes for wave selection
class WaveButton : public SymbolButton
{
public:
    WaveButton()
    {
        setClickingTogglesState (true);
        setStroke (true, 1.5f);
        setSymbolOnColor (UXPalette::highlight);
        setSymbolOffColor (UXPalette::darkGray);
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
class NoiseButton : public WaveButton
{
public:
    void setSymbol() override;
};
//===============================================================
class WaveSelector :
public juce::Component,
public juce::Button::Listener,
public juce::ComboBox::Listener
{
public:
    WaveSelector (int idx, apvts* tree, juce::String prefix);
    const int opIndex;
    apvts* const linkedTree;
    void resized() override;
    void buttonClicked (juce::Button* b) override;
    void comboBoxChanged (juce::ComboBox* b) override;
private:
    SineButton bSine;
    SquareButton bSquare;
    SawButton bSaw;
    TriButton bTri;
    NoiseButton bNoise;
    juce::ComboBox hiddenBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> hiddenBoxAttach;
};
//===============================================================
class OperatorComponent :
public juce::Component,
public juce::Button::Listener
{
public:
    OperatorComponent (int idx, apvts* tree, GraphParamSet* gParams);
    ~OperatorComponent();
    const int opIndex;
    apvts* const linkedTree;
    void resized() override;
    void buttonClicked (juce::Button* b) override;
    void paint (juce::Graphics& g) override;
private:
    EnvelopeComponent envComponent;
    WaveSelector waveSelect;
    OutputButton outButton;
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
    
    HexLookAndFeel hexLnf;
};
