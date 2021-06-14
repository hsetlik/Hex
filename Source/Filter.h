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
class FilterCore
{
public:
    FilterCore() :
    sampleRate(44100.0f),
    cutoff(2500.0f),
    resonance(1.0f)
    {
        
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


