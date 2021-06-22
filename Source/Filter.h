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

class HexFilter :
public juce::AsyncUpdater
{
public:
    HexFilter(int voiceIdx);
    void setSampleRate(double rate)
    {
        rateVal = rate;
        pFilter->setSampleRate(rateVal);
    }
    void setCutoff(float value)
    {
        cutoffVal = value;
        pFilter->setCutoff(cutoffVal);
    }
    void setResonance(float value)
    {
        resonanceVal = value;
        pFilter->setResonance(resonanceVal);
    }
    void handleAsyncUpdate() override;
    void setType(int filterType);
    float process(float input) {return pFilter->process(input); }
private:
    float cutoffVal;
    float resonanceVal;
    double rateVal;
    const int voiceIndex;
    FilterType currentType;
    std::unique_ptr<FilterCore> pFilter;
    
};




