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
#include "Filter.h"
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
    GraphParamSet() : lastTriggeredVoice(0)
    {
        
    }
    int lastTriggeredVoice;
    int pointFrequency;
    int pointIdx;
    std::atomic<float> levels[NUM_VOICES][NUM_OPERATORS];
    std::atomic<float> filterLevels[NUM_VOICES];
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
        setCurrentPlaybackSampleRate(newRate);
        voiceFilter.setSampleRate(newRate);
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
    StereoFilter voiceFilter;
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
    void setLevel(int idx, float value) {operators[idx]->setLevel(value); }
    void setWave(int idx, float value) {operators[idx]->setWave((int)value); }
    void clickDebug(float newSum, int currentSample, int sSample, int nSamples)
    {
        if(fabs(lastMonoSum - newSum) > 0.18f)
        {
            printf("Voice #%d clicked at sample index %d\n", voiceIndex, currentSample);
            printf("Difference: %f\n", lastMonoSum - newSum);
            printf("Start sample: %d\n", sSample);
            printf("Num Samples: %d\n\n", nSamples);
        }
        lastMonoSum = newSum;
    }
    bool anyEnvsActive()
    {
        for(auto op : operators)
        {
            if(op->envelope.isActive() && op->isAudible())
                return true;
        }
        return false;
    }
private:
    float sumL;
    float sumR;
    double fundamental;
    float lastMonoSum;
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
        for(auto voice : hexVoices)
        {
            voice->setSampleRate(newRate);
        }
    }
    //===============================================
    void updateRoutingForBlock();
    void updateEnvelopesForBlock();
    void updateOscillatorsForBlock();
    void updateFiltersForBlock();
    //! fucnitons for updating the voices based on parameter changes
    void setDelay(int idx, float value);
    void setAttack(int idx, float value);
    void setHold(int idx, float value);
    void setDecay(int idx, float value);
    void setSustain(int idx, float value);
    void setRelease(int idx, float value);
    //=================================================
    //! Filter Parameters
    void setDelayF(float value);
    void setAttackF(float value);
    void setHoldF(float value);
    void setDecayF(float value);
    void setSustainF(float value);
    void setReleaseF(float value);
    void setCutoff(float value);
    void setResonance(float value);
    void setWetDry(float value);
    void setDepth(float value);
    void setFilterType(float value);
    //===============================================
    void setRatio(int idx, float value);
    void setModIndex(int idx, float value);
    void setPan(int idx, float value);
    void setLevel(int idx, float value);
    void setAudible(int idx, bool value);
    void setWave(int idx, float value);
    //===============================================
    void prepareRingBuffer(int blockSize)
    {
        graphBuffer.setSize(2, blockSize * 10);
    }
    GraphParamSet graphParams;
    RingBuffer<GLfloat> graphBuffer;
private:
    RoutingGrid grid;
    std::vector<HexVoice*> hexVoices;
};
