/*
  ==============================================================================

    Filter.h
    Created: 14 Jun 2021 10:59:58am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "/Users/hayden/Desktop/Programming/JUCEProjects/Hex/Source/1.9.0/include/Iir.h"
#include <JuceHeader.h>
#include "DAHDSR.h"
#include "MathUtil.h"
#define CUTOFF_MIN 20.0f
#define CUTOFF_MAX 8000.0f
#define CUTOFF_DEFAULT 2500.0f
#define CUTOFF_CENTER 600.0f

#define RESONANCE_MIN 0.0f
#define RESONANCE_MAX 20.0f
#define RESONANCE_DEFAULT 1.0f
#define RESONANCE_CENTER 5.0f
enum FilterType
{
    LoPass,
    HiPass,
    BandPass
};

class FilterCore
{
public:
    FilterCore() :
    sampleRate(44100.0f),
    cutoff(2500.0f),
    resonance(1.0f)
    {
        setup();
    }
    virtual ~FilterCore() {}
    virtual float process(float input) {return 0.0f; }
    virtual void setup() {}
    virtual void setResonance(float value)
    {
        resonance = value;
        setup();
    }
    virtual void setCutoff(float value)
    {
        cutoff = value;
        setup();
    }
    virtual void setSampleRate(double rate)
    {
        sampleRate = rate;
        setup();
    }
protected:
    double sampleRate;
    float cutoff;
    float resonance;
};

class LibLowPass : public FilterCore
{
public:
    void setup() override
    {
        filter.setup(sampleRate, (double)cutoff, (double)resonance);
    }
    float process(float input) override
    {
        return filter.filter(input);
    }
private:
    Iir::RBJ::LowPass filter;
};

class LibHiPass : public FilterCore
{
public:
    void setup() override
    {
        filter.setup(sampleRate, (double)cutoff, (double)resonance);
    }
    float process(float input) override
    {
        return filter.filter(input);
    }
private:
    Iir::RBJ::HighPass filter;
};

class LibBandPass : public FilterCore
{
public:
    void setup() override
    {
        filter.setup(sampleRate, (double)cutoff, (double)resonance);
    }
    float process(float input) override
    {
        return filter.filter(input);
    }
private:
    Iir::RBJ::BandPass1 filter;
};

class StereoFilter :
public juce::AsyncUpdater
{
public:
    StereoFilter(int voiceIdx);
    void setSampleRate(double rate)
    {
        rateVal = rate;
        envelope.setSampleRate(rate);
        lFilter->setSampleRate(rateVal);
        rFilter->setSampleRate(rateVal);
    }
    void setCutoff(float value)
    {
        cutoffVal = value;
        lFilter->setCutoff(cutoffVal);
        rFilter->setCutoff(cutoffVal);
    }
    void setResonance(float value)
    {
        resonanceVal = value;
        lFilter->setResonance(resonanceVal);
        rFilter->setResonance(resonanceVal);
    }
    void setDepth(float value) {envDepth = value; }
    void setWetLevel(float value) {wetLevel = value; }
    void tick()
    {
        auto modVal = envelope.process(envDepth);
        auto inc = (CUTOFF_MAX - cutoffVal) * modVal;
        lFilter->setCutoff(cutoffVal + inc);
        rFilter->setCutoff(cutoffVal + inc);
    }
    void handleAsyncUpdate() override;
    void setType(int filterType);
    float processLeft(float input) {return MathUtil::fLerp(input, lFilter->process(input), wetLevel); }
    float processRight(float input) {return MathUtil::fLerp(input, rFilter->process(input), wetLevel); }
    DAHDSR envelope;
private:
    float cutoffVal;
    float resonanceVal;
    double rateVal;
    float envDepth;
    float wetLevel;
    const int voiceIndex;
    FilterType currentType;
    std::unique_ptr<FilterCore> lFilter;
    std::unique_ptr<FilterCore> rFilter;
};




