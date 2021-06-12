/*
  ==============================================================================

    Synthesizer.h
    Created: 4 Jun 2021 1:24:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "FMOperator.h"
#include "RingBuffer.h"
typedef std::array<std::array<float, NUM_OPERATORS>, NUM_VOICES> fVoiceOp;

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

class GraphParamSet
{
public:
    GraphParamSet()
    {

    }
    int lastTriggeredVoice;
    int pointFrequency;
    int pointIdx;
    fVoiceOp levels;
    std::array<float, NUM_OPERATORS> opRatios;
    std::array<float, NUM_VOICES> voiceFundamentals;
    std::array<bool, NUM_OPERATORS> opOutputs;
    std::array<float, NUM_OPERATORS> modIndeces;
    std::array<WaveType, NUM_OPERATORS> opWaves;
    RoutingGrid grid;
    std::array<float, 256> dataValues;
    void pushIn(float input)
    {
        dataValues[0] = input;
        for(int i = 255; i > 0; --i)
        {
            dataValues[i + 1] = dataValues[i];
        }
        
    }
private:
    int listLength;
    
};

class HexVoice : public juce::SynthesiserVoice
{
public:
    HexVoice(apvts* tree, GraphParamSet* gParams, RingBuffer<GLfloat>* buffer, int idx);
    apvts* const linkedTree;
    GraphParamSet* const linkedParams;
    RingBuffer<GLfloat>* const linkedBuffer;
    const int voiceIndex;
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<HexSound*>(sound) != nullptr;
    }
    void setSampleRate(double newRate)
    {
        for(auto op : operators)
            op->setSampleRate(newRate);
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
    //! function to make sure each operator has the appropriate offset before calculating samples
    void tickModulation();
    //! functions for setting parameters inside the operators
    void setDelay(int idx, float value) {operators[idx]->envelope.setDelay(value); }
    void setAttack(int idx, float value) {operators[idx]->envelope.setAttack(value); }
    void setHold(int idx, float value) {operators[idx]->envelope.setHold(value); }
    void setDecay(int idx, float value) {operators[idx]->envelope.setDecay(value); }
    void setSustain(int idx, float value) {operators[idx]->envelope.setSustain(value); }
    void setRelease(int idx, float value) {operators[idx]->envelope.setRelease(value); }
    //===============================================
    void setRatio(int idx, float value) {operators[idx]->setRatio(value); }
    void setModIndex(int idx, float value) {operators[idx]->setModIndex(value); }
    void setPan(int idx, float value) {operators[idx]->setPan(value); }
    void setAudible(int idx, bool value) {operators[idx]->setAudible(value); }
    void setWave(int idx, float value) {operators[idx]->setWave((int)value); }
private:
    float sumL;
    float sumR;
    double fundamental;
    RoutingGrid grid;
};

class HexSynth : public juce::Synthesiser
{
public:
    HexSynth(apvts* tree);
    ~HexSynth()
    {
    }
    apvts* const linkedTree;
    void setSampleRate(double newRate)
    {
        setCurrentPlaybackSampleRate(newRate);
        
    }
    //===============================================
    void updateRoutingForBlock();
    void updateEnvelopesForBlock();
    void updateOscillatorsForBlock();
    //! fucnitons for updating the voices based on parameter changes
    void setDelay(int idx, float value);
    void setAttack(int idx, float value);
    void setHold(int idx, float value);
    void setDecay(int idx, float value);
    void setSustain(int idx, float value);
    void setRelease(int idx, float value);
    //===============================================
    void setRatio(int idx, float value);
    void setModIndex(int idx, float value);
    void setPan(int idx, float value);
    void setAudible(int idx, bool value);
    void setWave(int idx, float value);
    //===============================================
    void prepareRingBuffer(int blockSize)
    {
        graphBuffer.setSize(1, blockSize * 10);
    }
    GraphParamSet graphParams;
    RingBuffer<GLfloat> graphBuffer;
private:
    RoutingGrid grid;
    std::vector<HexVoice*> hexVoices;
};
