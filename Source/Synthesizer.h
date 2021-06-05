/*
  ==============================================================================

    Synthesizer.h
    Created: 4 Jun 2021 1:24:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "FMOperator.h"

class HexSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) //just plays this sound for any midi note
    {
        return true;
    }
    bool appliesToChannel(int /*midiChannel*/) //plays the sound on both channels
    {
        return true;
    }
};

class HexVoice : public juce::SynthesiserVoice
{
public:
    HexVoice(apvts* tree);
    apvts* const linkedTree;
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<HexSound*>(sound) != nullptr;
    }
    void startNote(int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                   int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    //=============================================
    void updateGrid(RoutingGrid& newGrid){grid = newGrid; }
    //=============================================
    void pitchWheelMoved(int newPitchWheelVal) override {}
    //=============================================
    void controllerMoved(int controllerNumber, int controllerValue) override{}
    //===============================================
    void aftertouchChanged (int newAftertouchValue) override{}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override{}
    //===============================================
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    juce::OwnedArray<FMOperator> operators;
    //! functions for setting parameters inside the operators
    
private:
    RoutingGrid grid;
};

class HexSynth : public juce::Synthesiser
{
public:
    HexSynth(apvts* tree);
    apvts* const linkedTree;
    //! fucnitons for updating the voices based on parameter changes
    void setWave(int idx, WaveType type);
private:
    std::vector<HexVoice*> hexVoices;
};
