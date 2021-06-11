/*
  ==============================================================================

    WaveGraphComponent.h
    Created: 10 Jun 2021 2:37:56pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "Synthesizer.h"
#define WAVE_RES 256

class BasicOsc
{
public:
    BasicOsc(WaveType type);
    const WaveType oscType;
    std::array<float, WAVE_RES> waveData;
private:
    float phase;
    double baseFreq;
};

class WaveGraph :
public juce::AsyncUpdater,
public juce::Timer,
public juce::Component
{
public:
    WaveGraph(GraphParamSet* params);
    GraphParamSet* const linkedParams;
    void paint(juce::Graphics& g) override;
    void handleAsyncUpdate() override;
    void timerCallback() override {triggerAsyncUpdate(); }
    void updateTrace();
private:
    double fundamental;
    juce::Path trace;
    std::array<float, 256> wavePoints;
    
};
