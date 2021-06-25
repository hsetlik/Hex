/*
  ==============================================================================

    LfoComponent.h
    Created: 25 Jun 2021 4:43:39pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "OperatorComponent.h"
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
    static float periodHz(int num, int denom, float bpm)
    {
        auto beatsPerSecond = (float)bpm / 60.0f;
        auto lengthInBeats = ((float)num / (float)denom) * 4.0f;
        return beatsPerSecond * lengthInBeats;
    }
    float periodHz()
    {
        auto beatsPerSecond = (float)bpm / 60.0f;
        auto lengthInBeats = ((float)numerator / (float)denominator) * 4.0f;
        return beatsPerSecond * lengthInBeats;
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
private:
    void calculateHzValues();
    std::vector<float> hzValues;
    bool snapMode;
    float bpm;
    std::vector<std::pair<int, int>> noteLengths;
};


class LfoComponent :
public juce::Button::Listener
{
public:
    LfoComponent(juce::AudioProcessor* proc, GraphParamSet* gParams, apvts* tree);
    juce::AudioProcessor* const linkedProcessor;
    GraphParamSet* const linkedParams;
    apvts* const linkedTree;
    void buttonClicked(juce::Button* b) override;
private:
    juce::TextButton bpmToggle;
};
