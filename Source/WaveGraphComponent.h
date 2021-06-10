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
    juce::Path trace;
};
