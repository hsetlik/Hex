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
//! macros for use in parameter layout
#define RATIO_MIN 0.1f
#define RATIO_MAX 10.0f
#define RATIO_DEFAULT 1.0f
#define RATIO_CENTER 1.0f

#define MODINDEX_MIN 0.0f
#define MODINDEX_MAX 450.0f
#define MODINDEX_DEFAULT 0.0f
#define MODINDEX_CENTER 150.0f

#define PAN_MIN 0.0f
#define PAN_MAX 1.0f
#define PAN_DEFAULT 0.5f


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
    //! functions to set private variables
    void setRatio(float value) {baseRatio = value; }
    void setModIndex(float value) {modIndex = value; }
    void setPan(float value) {pan = value; }
    void setAudible(bool shouldBeAudible) {audible = shouldBeAudible; }
    void clearOffset() {modOffset = 0.0f; }
    const int index;
    //! access to variables
    bool isAudible() {return audible; }
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
    bool audible;
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
