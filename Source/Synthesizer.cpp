/*
  ==============================================================================

    Synthesizer.cpp
    Created: 4 Jun 2021 1:24:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"
HexVoice::HexVoice(apvts* tree, GraphParamSet* gParams, RingBuffer<GLfloat>* buffer, int idx) :
linkedTree(tree),
linkedParams(gParams),
linkedBuffer(buffer),
voiceIndex(idx),
sumL(0.0f),
sumR(0.0f),
fundamental(0.0f),
lastMonoSum(0.0f)
{
    for(int i = 0; i < NUM_OPERATORS; ++i)
    {
        operators.add(new FMOperator(i));
    }
}

void HexVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    linkedParams->lastTriggeredVoice = voiceIndex;
    linkedParams->voiceFundamentals[voiceIndex] = (float)fundamental;
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
    for(int i = startSample; i < (startSample + numSamples); ++i)
    {
        for(auto o : operators)
            o->clearOffset();
        tickModulation();
        sumL = 0.0f;
        sumR = 0.0f;
        for(int op = 0; op < NUM_OPERATORS; ++op)
        {
            operators[op]->tick(fundamental);
            linkedParams->levels[voiceIndex][op] = operators[op]->envelope.getLastLevel();
            if(operators[op]->isAudible())
            {
                sumL += operators[op]->lastLeft();
                sumR += operators[op]->lastRight();
            }
            ++linkedParams->pointIdx;
        }
        outputBuffer.addSample(0, i, sumL);
        outputBuffer.addSample(1, i, sumR);
    }
    if(linkedParams->lastTriggeredVoice == voiceIndex)
    {
        linkedBuffer->writeSamples(outputBuffer, startSample, numSamples);
    }
}
//=====================================================================================================================
void HexVoice::tickModulation()
{
    for(int o = 0; o < NUM_OPERATORS; ++o)
    {
        for(int i = 0; i < NUM_OPERATORS; ++i)
        {
            if(grid[o][i])
            {
                operators[i]->addModFrom(*operators[o]);
            }
        }
    }
}
//=====================================================================================================================
HexSynth::HexSynth(apvts* tree) :
linkedTree(tree),
graphBuffer(2, 256 * 10)
{
    for(int i = 0; i < NUM_VOICES; ++i)
    {
        addVoice(new HexVoice(linkedTree, &graphParams, &graphBuffer, i));
        auto* voice = dynamic_cast<HexVoice*>(voices.getLast());
        hexVoices.push_back(voice);
    }
    addSound(new HexSound);
}
//=====================================================================================================================
void HexSynth::setDelay(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setDelay(idx, value);
    }
}
void HexSynth::setAttack(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setAttack(idx, value);
    }
}
void HexSynth::setHold(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setHold(idx, value);
    }
}
void HexSynth::setDecay(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setDecay(idx, value);
    }
}
void HexSynth::setSustain(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setSustain(idx, value);
    }
}
void HexSynth::setRelease(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setRelease(idx, value);
    }
}
void HexSynth::setAudible(int idx, bool value)
{
    for(auto voice : hexVoices)
    {
        voice->setAudible(idx, value);
    }
}
void HexSynth::setModIndex(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setModIndex(idx, value);
    }
}
void HexSynth::setRatio(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setRatio(idx, value);
    }
}
void HexSynth::setPan(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setPan(idx, value);
    }
}
void HexSynth::setWave(int idx, float value)
{
    for(auto voice : hexVoices)
    {
        voice->setWave(idx, value);
    }
}
//===========================================================================
void HexSynth::updateRoutingForBlock()
{
    for(int o = 0; o < NUM_OPERATORS; ++o)
    {
        auto oStr = juce::String(o);
        for(int i = 0; i < NUM_OPERATORS; ++i)
        {
            auto iStr = juce::String(i);
            auto str = oStr + "to" + iStr + "Param";
            if(linkedTree->getRawParameterValue(str)->load() > 0.0f)
                grid[o][i] = true;
            else
                grid[o][i] = false;
        }
    }
    graphParams.grid = grid;
    for(auto voice : hexVoices)
    {
        voice->updateGrid(grid);
    }
}

void HexSynth::updateEnvelopesForBlock()
{
    for(int i = 0 ; i < NUM_OPERATORS; ++i)
    {
        auto iStr = juce::String(i);
        auto delayId = "delayParam" + iStr;
        auto attackId = "attackParam" + iStr;
        auto holdId = "holdParam" + iStr;
        auto decayId = "decayParam" + iStr;
        auto sustainId = "sustainParam" + iStr;
        auto releaseId = "releaseParam" + iStr;
        float delay = *linkedTree->getRawParameterValue(delayId);
        float attack = *linkedTree->getRawParameterValue(attackId);
        float hold = *linkedTree->getRawParameterValue(holdId);
        float decay = *linkedTree->getRawParameterValue(decayId);
        float sustain = *linkedTree->getRawParameterValue(sustainId);
        float release = *linkedTree->getRawParameterValue(releaseId);
        setDelay(i, delay);
        setAttack(i, attack);
        setHold(i, hold);
        setDecay(i, decay);
        setSustain(i, sustain);
        setRelease(i, release);
    }
}

void HexSynth::updateOscillatorsForBlock()
{
    for(int i = 0; i < NUM_OPERATORS; ++i)
    {
        auto iStr = juce::String(i);
        auto ratioId = "ratioParam" + iStr;
        auto indexId = "indexParam" + iStr;
        auto outputId = "audibleParam" + iStr;
        auto panId = "panParam" + iStr;
        auto waveId = "waveParam" + iStr;
        float ratio = *linkedTree->getRawParameterValue(ratioId);
        graphParams.opRatios[i] = ratio;
        float modIndex = *linkedTree->getRawParameterValue(indexId);
        graphParams.modIndeces[i] = modIndex;
        bool audible = (*linkedTree->getRawParameterValue(outputId) > 0.0f);
        float pan = *linkedTree->getRawParameterValue(panId);
        float wave = *linkedTree->getRawParameterValue(waveId);
        setRatio(i, ratio);
        setModIndex(i, modIndex);
        setAudible(i, audible);
        setPan(i, pan);
        setWave(i, wave);
    }
}
