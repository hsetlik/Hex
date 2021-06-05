/*
  ==============================================================================

    FMOperator.h
    Created: 4 Jun 2021 2:15:51pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "FMOscillator.h"
#include "DAHDSR.h"
#define NUM_OPERATORS 6
#define NUM_VOICES 6

using RoutingGrid = std::array<std::array<bool, NUM_OPERATORS>, NUM_OPERATORS>;
using apvts = juce::AudioProcessorValueTreeState;
class FMOperator
{
public:
    FMOperator(int opIndex);
    void trigger(bool on)
    {
        if(on)
            envelope.triggerOn();
        else
            envelope.triggerOff();
    }
    void setSampleRate(double rate);
    void clearOffset() {modOffset = 0.0f; }
    const int index;
    //! access to variables
    float lastMono() {return lastOutMono; }
    float lastLeft() {return lastOutL; }
    float lastRight() {return lastOutR; }
    //! where the magic happens
    void addModFrom(FMOperator& source)
    {
        modOffset += source.lastMono();
    }
    void tick(double fundamental);
    HexOsc oscillator;
    DAHDSR envelope;
private:
    //! Settable parameters stored down here
    float modIndex;
    float baseRatio;
    float modOffset;
    float pan;
    //!  output variables
    float lastOutMono;
    float lastOutL;
    float lastOutR;
};
