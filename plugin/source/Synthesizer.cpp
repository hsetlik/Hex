/*
  ==============================================================================

    Synthesizer.cpp
    Created: 4 Jun 2021 1:24:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"
#include "Identifiers.h"
#include "juce_core/juce_core.h"
HexVoice::HexVoice(apvts* tree,
                   GraphParamSet* gParams,
                   RingBuffer<float>* buffer,
                   int idx)
    : linkedTree(tree),
      linkedParams(gParams),
      linkedBuffer(buffer),
      voiceIndex(idx),
      voiceFilter(voiceIndex),
      justKilled(false),
      internalBuffer(2, 512),
      sumL(0.0f),
      sumR(0.0f),
      fundamental(0.0f),
      voiceCleared(true),
      magnitude(0.0f),
      lastMagnitude(0.0f) {
  for (int i = 0; i < NUM_OPERATORS; ++i) {
    operators.add(new FMOperator(i));
  }
  for (int i = 0; i < NUM_LFOS; ++i) {
    lfos.add(new HexLfo(i));
  }
}

void HexVoice::startNote(int midiNoteNumber,
                         float velocity,
                         juce::SynthesiserSound*,
                         int) {
  voiceCleared = false;
  fundamental = MathUtil::midiToET(midiNoteNumber);
  linkedParams->lastTriggeredVoice.store(voiceIndex);
  ++linkedParams->voicesInUse;
  linkedParams->voiceFundamentals[voiceIndex].store((float)fundamental);
  voiceFilter.envelope.triggerOn();
  for (auto op : operators) {
    op->trigger(true);
  }
  debugPrinter.addMessage("Voice " + juce::String(voiceIndex) + " started");
}

void HexVoice::stopNote(float velocity, bool allowTailOff) {
  juce::ignoreUnused(velocity);
  voiceFilter.envelope.triggerOff();
  for (auto op : operators) {
    op->trigger(false);
  }
  if (!allowTailOff) {
    killQuick();
  }
  if (linkedParams->voicesInUse.load() > 0) {
    --linkedParams->voicesInUse;
  }
}

void HexVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                               int startSample,
                               int numSamples) {
  internalBuffer.clear();
  if (outputBuffer.getNumSamples() > internalBuffer.getNumSamples())
    internalBuffer.setSize(2, outputBuffer.getNumSamples());
  for (int i = startSample; i < (startSample + numSamples); ++i) {
    for (auto op : operators)
      op->clearOffset();
    tickModulation();
    voiceFilter.tick();
    sumL = 0.0f;
    sumR = 0.0f;
    int idx = 0;
    for (auto op : operators) {
      op->tick(fundamental, levelMod(idx));
      if (op->isAudible()) {
        sumL += op->lastLeft();
        sumR += op->lastRight();
      }
      ++idx;
    }
    filterValue = filterMod();
    if (filterValue > 0.0f) {
      sumL = voiceFilter.processLeft(sumL, filterValue);
      sumR = voiceFilter.processRight(sumR, filterValue);
    } else {
      sumL = voiceFilter.processLeft(sumL);
      sumR = voiceFilter.processRight(sumR);
    }
    internalBuffer.setSample(0, i, sumR);
    internalBuffer.setSample(1, i, sumL);
  }
  outputBuffer.addFrom(0, startSample, internalBuffer, 0, startSample,
                       numSamples);
  outputBuffer.addFrom(1, startSample, internalBuffer, 1, startSample,
                       numSamples);
  //! handle sending data to the graphing stuff
  for (int op = 0; op < NUM_OPERATORS; ++op) {
    linkedParams->levels[voiceIndex][op].store(
        operators[op]->envelope.getLastLevel());
    linkedParams->filterLevels[voiceIndex].store(
        voiceFilter.envelope.getLastLevel());
  }
  if (linkedParams->lastTriggeredVoice == voiceIndex) {
    linkedBuffer->writeSamples(internalBuffer, startSample, numSamples);
  }
  if (!anyEnvsActive()) {
    clearCurrentNote();
    voiceCleared = true;
    debugPrinter.addMessage("Voice " + juce::String(voiceIndex) + " cleared");
  }
}
//=====================================================================================================================
void HexVoice::tickModulation() {
  for (int o = 0; o < NUM_OPERATORS; ++o) {
    for (int i = 0; i < NUM_OPERATORS; ++i) {
      if (grid[o][i]) {
        operators[i]->addModFrom(*operators[o]);
      }
    }
  }
}
//=====================================================================================================================
HexSynth::HexSynth(apvts* tree)
    : linkedTree(tree),
      graphBuffer(2, 256 * 10),
      magnitude(0.0f),
      lastMagnitude(0.0f),
      numJumps(0) {
  for (int i = 0; i < NUM_VOICES; ++i) {
    addVoice(new HexVoice(linkedTree, &graphParams, &graphBuffer, i));
    auto* voice = dynamic_cast<HexVoice*>(voices.getLast());
    hexVoices.push_back(voice);
  }
  addSound(new HexSound);
  setNoteStealingEnabled(false);
}
/*
juce::SynthesiserVoice* HexSynth::findFreeVoice (juce::SynthesiserSound
*soundToPlay, int midiChannel, int midiNoteNum, bool stealIfNoneAvailible) const
{
    auto idx = 0;
    for (auto v : hexVoices)
    {
        if (v->isVoiceCleared())
        {
            return v;
        }
        ++idx;
    }
    return nullptr;
}


void HexSynth::noteOn (int midiChannel, int midiNoteNumber, float velocity)
{
    const juce::ScopedLock sl (lock);
    for (auto* sound : sounds)
    {
        if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel
(midiChannel))
        {
            //! if a voice is already playing this note, stop it
            for (auto* voice : hexVoices)
            {
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber &&
voice->isPlayingChannel (midiChannel))
                {
                    stopVoice (voice, 1.0f, true);
                    voice->justKilled = true;
                }
                else
                    voice->justKilled = false;
            }
            auto* freeVoice = getFreeVoice (midiChannel, midiNoteNumber);
            if (freeVoice != nullptr)
            {
                startVoice (freeVoice, sound, midiChannel, midiNoteNumber,
velocity);
            }
            else
            {
                printer.addMessage("Free Voice is nullptr!");
            }

        }
    }
}
void HexSynth::noteOff (int midiChannel, int midiNoteNumber, float velocity,
bool allowTailOff)
{
    const juce::ScopedLock sl (lock);
    for (auto* voice : voices)
    {
        if (voice->getCurrentlyPlayingNote() == midiNoteNumber
              && voice->isPlayingChannel (midiChannel))
        {
            if (auto sound = voice->getCurrentlyPlayingSound())
            {
                if (sound->appliesToNote (midiNoteNumber)
                     && sound->appliesToChannel (midiChannel))
                {
                    voice->setKeyDown (false);
                    if (! (voice->isSustainPedalDown() ||
voice->isSostenutoPedalDown())) stopVoice (voice, velocity, allowTailOff);
                }
            }
        }
    }
}
*/
void HexSynth::renderVoices(juce::AudioBuffer<float>& buffer,
                            int startSample,
                            int numSamples) {
  const juce::ScopedLock sl(lock);
  for (auto v : hexVoices) {
    if (!v->isVoiceCleared())
      v->renderNextBlock(buffer, startSample, numSamples);
  }
}

void HexSynth::handleBlock(juce::AudioBuffer<float>& buffer,
                           const juce::MidiBuffer& midi,
                           int startSample,
                           int numSamples) {}
//=====================================================================================================================
void HexSynth::setRate(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto v : hexVoices) {
    v->lfos[idx]->setRate(value);
  }
}
void HexSynth::setDepth(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto v : hexVoices) {
    v->lfoDepths[idx] = value;
  }
}
void HexSynth::setTarget(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto v : hexVoices) {
    v->lfoTargets[idx] = value;
  }
}
void HexSynth::setLfoWave(int idx, float value) {
  for (auto v : hexVoices)
    v->lfos[idx]->setType((int)value);
}
//=====================================================================================================================
void HexSynth::setDelay(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setDelay(idx, value);
  }
}
void HexSynth::setAttack(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setAttack(idx, value);
  }
}
void HexSynth::setHold(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setHold(idx, value);
  }
}
void HexSynth::setDecay(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setDecay(idx, value);
  }
}
void HexSynth::setSustain(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setSustain(idx, value);
  }
}
void HexSynth::setRelease(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setRelease(idx, value);
  }
}
void HexSynth::setDelayF(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.envelope.setDelay(value);
  }
}
void HexSynth::setAttackF(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.envelope.setAttack(value);
  }
}
void HexSynth::setHoldF(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.envelope.setHold(value);
  }
}
void HexSynth::setDecayF(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.envelope.setDecay(value);
  }
}
void HexSynth::setSustainF(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.envelope.setSustain(value);
  }
}
void HexSynth::setReleaseF(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.envelope.setRelease(value);
  }
}
void HexSynth::setCutoff(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.setCutoff(value);
  }
}
void HexSynth::setResonance(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.setResonance(value);
  }
}
void HexSynth::setWetDry(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.setWetLevel(value);
  }
}
void HexSynth::setDepth(float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->voiceFilter.setDepth(value);
  }
}
void HexSynth::setFilterType(float value) {
  const juce::ScopedLock sl(lock);
  auto tVal = (int)value;
  for (auto voice : hexVoices) {
    voice->voiceFilter.setType(tVal);
  }
}
//============================================================
void HexSynth::setAudible(int idx, bool value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setAudible(idx, value);
  }
}
void HexSynth::setModIndex(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setModIndex(idx, value);
  }
}
void HexSynth::setRatio(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setRatio(idx, value);
  }
}
void HexSynth::setPan(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setPan(idx, value);
  }
}

void HexSynth::setLevel(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setLevel(idx, value);
  }
}

void HexSynth::setWave(int idx, float value) {
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->setWave(idx, value);
  }
}
//===========================================================================
void HexSynth::updateRoutingForBlock() {
  for (size_t o = 0; o < NUM_OPERATORS; ++o) {
    auto oStr = juce::String(o);
    for (size_t i = 0; i < NUM_OPERATORS; ++i) {
      auto iStr = juce::String(i);
      auto str = oStr + "to" + iStr + "Param";
      if (*linkedTree->getRawParameterValue(str) > 0.0f)
        grid[o][i] = true;
      else
        grid[o][i] = false;
    }
  }
  const juce::ScopedLock sl(lock);
  for (auto voice : hexVoices) {
    voice->updateGrid(grid);
  }
}

void HexSynth::updateEnvelopesForBlock() {
  for (int i = 0; i < NUM_OPERATORS; ++i) {
    auto iStr = juce::String(i);
    auto delayId = ID::envDelay + iStr;
    auto attackId = ID::envAttack + iStr;
    auto holdId = ID::envHold + iStr;
    auto decayId = ID::envDecay + iStr;
    auto sustainId = ID::envSustain + iStr;
    auto releaseId = ID::envRelease + iStr;
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
void HexSynth::updateOscillatorsForBlock() {
  for (int i = 0; i < NUM_OPERATORS; ++i) {
    auto iStr = juce::String(i);
    auto ratioId = ID::operatorRatio + iStr;
    auto indexId = ID::operatorModIndex + iStr;
    auto outputId = ID::operatorAudible + iStr;
    auto panId = ID::operatorPan + iStr;
    auto waveId = ID::operatorWaveShape + iStr;
    auto levelId = ID::operatorLevel + iStr;
    float ratio = *linkedTree->getRawParameterValue(ratioId);
    float modIndex = *linkedTree->getRawParameterValue(indexId);
    bool audible = (*linkedTree->getRawParameterValue(outputId) > 0.0f);
    float pan = *linkedTree->getRawParameterValue(panId);
    float level = *linkedTree->getRawParameterValue(levelId);
    float wave = *linkedTree->getRawParameterValue(waveId);
    setRatio(i, ratio);
    setModIndex(i, modIndex);
    setAudible(i, audible);
    setPan(i, pan);
    setLevel(i, level);
    setWave(i, wave);
  }
}
void HexSynth::updateFiltersForBlock() {
  float delay = *linkedTree->getRawParameterValue(ID::filterEnvDelay);
  float attack = *linkedTree->getRawParameterValue(ID::filterEnvAttack);
  float hold = *linkedTree->getRawParameterValue(ID::filterEnvHold);
  float decay = *linkedTree->getRawParameterValue(ID::filterEnvDecay);
  float sustain = *linkedTree->getRawParameterValue(ID::filterEnvSustain);
  float release = *linkedTree->getRawParameterValue(ID::filterEnvRelease);
  setDelayF(delay);
  setAttackF(attack);
  setHoldF(hold);
  setDecayF(decay);
  setSustainF(sustain);
  setReleaseF(release);

  float cutoff = *linkedTree->getRawParameterValue(ID::filterCutoff);
  float resonance = *linkedTree->getRawParameterValue(ID::filterResonance);
  float wet = *linkedTree->getRawParameterValue(ID::filterWetDry);
  float depth = *linkedTree->getRawParameterValue(ID::filterEnvDepth);
  float type = *linkedTree->getRawParameterValue(ID::filterType);
  setCutoff(cutoff);
  setResonance(resonance);
  setWetDry(wet);
  setDepth(depth);
  setFilterType(type);
}

void HexSynth::updateLfosForBlock() {
  for (int i = 0; i < NUM_LFOS; ++i) {
    auto iStr = juce::String(i);
    float rate = *linkedTree->getRawParameterValue(ID::lfoRate + iStr);
    float depth = *linkedTree->getRawParameterValue(ID::lfoDepth + iStr);
    float target = *linkedTree->getRawParameterValue(ID::lfoTarget + iStr);
    float wave = *linkedTree->getRawParameterValue(ID::lfoWave + iStr);
    setRate(i, rate);
    setDepth(i, depth);
    setTarget(i, target);
    setLfoWave(i, wave);
  }
}
