/*
  ==============================================================================

    LFO.cpp
    Created: 22 Jun 2021 2:13:30pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LFO.h"

LfoArray WaveArray::arrayForType(WaveType type)
{
    LfoArray array;
    switch(type)
    {
        case Sine:
        {
            auto dPhase = juce::MathConstants<float>::twoPi / (float)TABLESIZE;
            for(int i = 0; i < TABLESIZE; ++i)
            {
                array[i] = (std::sin(dPhase * i) / 2.0f) + 0.5f;
            }
            break;
        }
        case Square:
        {
            for(int i = 0; i < TABLESIZE; ++i)
            {
                if(i < TABLESIZE / 2)
                    array[i] = 1.0f;
                else
                    array[i] = 0.0f;
            }
            break;
        }
        case Saw:
        {
            auto dY = 1.0f / (float)TABLESIZE;
            for(int i = 0; i < TABLESIZE; ++i)
            {
                array[i] = i * dY;
            }
            break;
        }
        case Tri:
        {
            auto dY = 2.0f / TABLESIZE;
            float level = 0.0f;
            for(int i = 0; i < TABLESIZE; ++i)
            {
                if(i < TABLESIZE / 2)
                    level += dY;
                else
                    level -= dY;
                array[i] = level;
            }
            break;
        }
        case Noise:
        {
            break;
        }
    }
    return array;
}
//====================================================================================
HexLfo::HexLfo(int idx) :
lfoIndex(idx),
currentType(Sine),
pOsc(std::make_unique<WaveLfo>(currentType)),
sampleRate(44100.0f),
rate(1.0f)
{
    
}
void HexLfo::setType(int type)
{
    //! safe to call this on every block bc replacement code only runs when the type changes
    if(type != (int)currentType)
    {
        currentType = (WaveType)type;
        triggerAsyncUpdate();
    }
}

void HexLfo::handleAsyncUpdate()
{
    if(currentType != Noise)
        pOsc.reset(new WaveLfo(currentType));
    else
        pOsc.reset(new NoiseLfo());
    //! Make sure other parameters stay the same when the type gets changed
    pOsc->setSampleRate(sampleRate);
    pOsc->setRate(rate);
}

void HexLfo::setRate(float _rate)
{
    rate = _rate;
    pOsc->setRate(rate);
}
void HexLfo::setSampleRate(double rate)
{
    sampleRate = rate;
    pOsc->setSampleRate(rate);
}
float HexLfo::tick()
{
    return pOsc->tick();
}
float HexLfo::tickToValue(float baseValue, float maxValue, float depth)
{
    return MathUtil::fLerp(baseValue, maxValue, tick() * depth);
}

