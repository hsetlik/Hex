/*
  ==============================================================================

    Synthesizer.cpp
    Created: 4 Jun 2021 1:24:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"
HexVoice::HexVoice(apvts* tree) : linkedTree(tree)
{
    for(int i = 0; i < NUM_OPERATORS; ++i)
    {
        operators.add(new FMOperator(i));
    }
}

void HexVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    for(auto op : operators)
    {
        op->trigger(true);
    }
}

void HexVoice::stopNote(float velocity, bool allowTailOff)
{
    for(auto op : operators)
    {
        op->trigger(false);
    }
}

void HexVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
}
//=====================================================================================================================
HexSynth::HexSynth(apvts* tree) : linkedTree(tree)
{
    for(int i = 0; i < NUM_VOICES; ++i)
    {
        addVoice(new HexVoice(linkedTree));
        auto* voice = dynamic_cast<HexVoice*>(voices.getLast());
        hexVoices.push_back(voice);
    }
    addSound(new HexSound);
}
