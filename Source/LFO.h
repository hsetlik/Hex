/*
  ==============================================================================

    LFO.h
    Created: 22 Jun 2021 2:13:30pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "MathUtil.h"
#include "FMOscillator.h"

typedef std::array<float, TABLESIZE> LfoArray;

class WaveArray
{
public:
    static LfoArray arryForType(WaveType type);
};

class LfoBase
{
public:
    LfoBase() : rate(1.0f), sampleRate(44100.0f)
    {
    }
    virtual ~LfoBase() {}
    virtual void setSampleRate(double rate) {sampleRate = rate; }
    virtual void setRate(float speedHz) {rate = speedHz; }
    virtual float tick() {}
protected:
    float rate;
    double sampleRate;
};

class WaveLfo : public LfoBase
{
public:
    WaveLfo(WaveType type) :
    wave(type),
    data(WaveArray::arrayForType(type)),
    phase(0.0f)
    {
    }
    float tick() override
    {
        phaseDelta = rate / sampleRate;
        phase += phaseDelta;
        if(phase > 1.0f)
            phase -= 1.0f;
        lowerIdx = std::floor(phase * TABLESIZE);
        upperIdx = (lowerIdx == TABLESIZE - 1) ? 0 : lowerIdx + 1;
        skew = (phase * TABLESIZE) - lowerIdx;
        return MathUtil::fLerp(data[lowerIdx], data[upperIdx], skew);
    }
private:
    const WaveType wave;
    LfoArray data;
    float phase;
    float phaseDelta;
    int lowerIdx;
    int upperIdx;
    float skew;
};

class NoiseLfo : public LfoBase
{
public:
    NoiseLfo() : rGen(12)
    {
        output = rGen.nextFloat();
    }
    float tick() override
    {
        phaseDelta = rate / sampleRate;
        phase += phaseDelta;
        if(phase > 1.0f)
        {
            phase -= 1.0f;
            output = rGen.nextFloat();
        }
        return output;
    }
private:
    juce::Random rGen;
    float output;
};

class HexLfo : public juce::AsyncUpdater
{
public:
    HexLfo(int idx);
    const int lfoIndex;
    float tick();
    float tickToValue(float baseValue, float maxValue, float depth);
    void setSampleRate(double rate);
    void setRate(float rate);
    void setType(WaveType type);
    void handleAsyncUpdate() override;
private:
    WaveType currentType;
    std::unique_ptr<LfoBase> pOsc;
    double sampleRate;
    float rate;
};
