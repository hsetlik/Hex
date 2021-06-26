/*
  ==============================================================================

    LfoComponent.h
    Created: 25 Jun 2021 4:43:39pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "OperatorComponent.h"
 /* Each LFO's APVTS Parametes are:
  lfoWaveParam: int, range 0-4
  lfoRateParam: float, range 0.1 - 20 (in hZ)
  lfoSyncParam: bool, controls whether LFO should be in sync or hz mode
  lfoTargetParam: int, range 0 - NUM_OPERATORS + 1
  lfoDepthParam: float, range 0 - 1
  
  lfoTargets should be laid out like:
  0: none
  1 - NUM_OPERATORS + 1: operator levels
  NUM_OPERATORS + 2: filter cutoff
  */
using PosInfo = juce::AudioPlayHead::CurrentPositionInfo;
struct NoteLength
{
    int numerator;
    int denominator;
    float bpm;
    NoteLength(int num, int denom, float _bpm) :
    numerator(num),
    denominator(denom),
    bpm(_bpm)
    {
    }
    static float frequencyHz(int num, int denom, float bpm)
    {
        auto beatsPerSecond = (float)bpm / 60.0f;
        auto lengthInBeats = ((float)num / (float)denom) * 4.0f;
        auto period = beatsPerSecond * lengthInBeats;
        return 1.0f / period;
    }
    float frequencyHz()
    {
        auto beatsPerSecond = (float)bpm / 60.0f;
        auto lengthInBeats = ((float)numerator / (float)denominator) * 4.0f;
        auto period = beatsPerSecond * lengthInBeats;
        return 1.0f / period;
    }
};

class DualModeSlider : public juce::Slider
{
public:
    DualModeSlider();
    void setBpm(float val) {bpm = val; calculateHzValues(); }
    double snapValue(double attemptedValue, juce::Slider::DragMode dragMode) override;
    bool inSnapMode() {return snapMode; }
    void toggleSnapMode() {snapMode = !snapMode; }
    std::pair<int, int> currentNoteLength();
    bool setSync(int num, int denom);
private:
    void calculateHzValues();
    std::vector<float> hzValues;
    bool snapMode;
    float bpm;
    std::vector<std::pair<int, int>> noteLengths;
};

class DualModeLabel :
public juce::Label,
public juce::Slider::Listener
{
public:
    DualModeLabel(DualModeSlider* s);
    DualModeSlider* const linkedSlider;
    void sliderValueChanged(juce::Slider* s) override;
    void textWasEdited() override;
    void setTextHz(float value);
    void setTextNoteLength(int num, int denom);
private:
    juce::String lastStr;
};


class LfoComponent :
public juce::Component,
public juce::Button::Listener,
public juce::Timer
{
public:
    LfoComponent(int i, juce::AudioProcessor* proc, GraphParamSet* gParams, apvts* tree);
    const int lfoIndex;
    juce::AudioProcessor* const linkedProcessor;
    GraphParamSet* const linkedParams;
    apvts* const linkedTree;
    void buttonClicked(juce::Button* b) override;
    void timerCallback() override;
    void prepare(); //! call this in the PrepareToPlay method in the processor, it should set the BPM for the slider
    void resized() override;
    static juce::StringArray getTargetStrings();
private:
    juce::TextButton bpmToggle;
    juce::ComboBox targetBox;
    DualModeSlider rateSlider;
    DualModeLabel rateLabel;
    PosInfo currentPos;
    WaveSelector waveSelect;
    pSliderAttach rateAttach;
    pButtonAttach syncAttach;
    pComboBoxAttach targetAttach;
    float bpm;
};
