/*
  ==============================================================================

    Synthesizer.h
    Created: 4 Jun 2021 1:24:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "DAHDSR.h"
#include "DebugUtil.h"
#include "FMOperator.h"
#include "Filter.h"
#include "LFO.h"
#include "RingBuffer.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
typedef std::array<std::array<float, NUM_OPERATORS>, NUM_VOICES> fVoiceOp;

class HexSound : public juce::SynthesiserSound {
public:
  bool appliesToNote(
      int /*midiNoteNumber*/)  // just plays this sound for any midi note
  {
    return true;
  }
  bool appliesToChannel(
      int /*midiChannel*/)  // plays the sound on both channels
  {
    return true;
  }
};

class GraphParamSet {
public:
  GraphParamSet() : lastTriggeredVoice(0), voicesInUse(0) {
    for (int i = 0; i < NUM_VOICES; ++i) {
      filterLevels[i].store(0.0f);
      voiceFundamentals[i].store(0.0f);
      for (int n = 0; n < NUM_OPERATORS; ++n) {
        levels[i][n].store(0.0f);
      }
    }
  }
  std::atomic<int> lastTriggeredVoice;
  std::atomic<int> voicesInUse;
  std::atomic<float> levels[NUM_VOICES][NUM_OPERATORS];
  std::atomic<float> filterLevels[NUM_VOICES];
  std::atomic<float> voiceFundamentals[NUM_VOICES];
};

class HexVoice : public juce::SynthesiserVoice {
public:
  HexVoice(apvts* tree,
           GraphParamSet* gParams,
           RingBuffer<float>* buffer,
           int idx,
           EnvelopeLUTGroup* envLuts);
  apvts* const linkedTree;
  GraphParamSet* const linkedParams;
  RingBuffer<float>* const linkedBuffer;
  const int voiceIndex;
  void prepareBuffer(int blockSize) { internalBuffer.setSize(2, blockSize); }
  bool canPlaySound(juce::SynthesiserSound* sound) override {
    return dynamic_cast<HexSound*>(sound) != nullptr;
  }
  void setSampleRate(double newRate, int blockSize = 512) {
    juce::ignoreUnused(blockSize);
    setCurrentPlaybackSampleRate(newRate);
    voiceFilter.setSampleRate(newRate);
    for (auto op : operators)
      op->setSampleRate(newRate);
    for (auto lfo : lfos)
      lfo->setSampleRate(newRate);
  }
  void startNote(int midiNoteNumber,
                 float velocity,
                 juce::SynthesiserSound* sound,
                 int currentPitchWheelPosition) override;
  void nStartNote(int midiNoteNumber, float velocity, int pitchWheelPos);
  void stopNote(float velocity, bool allowTailOff) override;
  //=============================================
  void updateGrid(RoutingGrid& newGrid) { grid = newGrid; }
  //=============================================
  void pitchWheelMoved(int) override {}
  //=============================================
  void controllerMoved(int, int) override {}
  //===============================================
  void aftertouchChanged(int) override {}
  //==============================================
  void channelPressureChanged(int) override {}
  //===============================================
  void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                       int startSample,
                       int numSamples) override;
  juce::OwnedArray<FMOperator> operators;
  juce::OwnedArray<HexLfo> lfos;
  StereoFilter voiceFilter;
  //! function to make sure each operator has the appropriate offset before
  //! calculating samples
  void tickModulation();

  //===============================================
  void setRatio(int idx, float value) { operators[idx]->setRatio(value); }
  void setModIndex(int idx, float value) { operators[idx]->setModIndex(value); }
  void setPan(int idx, float value) { operators[idx]->setPan(value); }
  void setAudible(int idx, bool value) { operators[idx]->setAudible(value); }
  void setLevel(int idx, float value) { operators[idx]->setLevel(value); }
  void setWave(int idx, float value) { operators[idx]->setWave((int)value); }
  bool anyEnvsActive() {
    for (auto op : operators) {
      if (op->vEnv.isActive())
        return true;
    }
    return false;
  }
  void killQuick() {
    for (auto op : operators) {
      op->vEnv.killQuick();
    }
    voiceFilter.env.killQuick();
  }
  bool isVoiceCleared() { return voiceCleared; }
  float filterMod() {
    for (int i = 0; i < NUM_LFOS; ++i) {
      if (lfoTargets[i] == NUM_OPERATORS + 1)
        return lfos[i]->tick() * lfoDepths[i];
    }
    return 0.0f;
  }
  float levelMod(int opIdx) {
    for (int i = 0; i < NUM_LFOS; ++i) {
      if (lfoTargets[i] == opIdx + 1)
        return lfos[i]->tick() * lfoDepths[i];
    }
    return 0.0f;
  }
  bool justKilled;
  int lfoTargets[NUM_LFOS];
  float lfoDepths[NUM_LFOS];
  float lfoValues[NUM_LFOS];

private:
  AsyncDebugPrinter debugPrinter;
  juce::AudioBuffer<float> internalBuffer;
  float sumL;
  float sumR;
  double fundamental;
  RoutingGrid grid;
  bool voiceCleared;
  float magnitude;
  float lastMagnitude;
  float filterValue;
};

class HexSynth : public juce::Synthesiser {
public:
  HexSynth(apvts* tree);
  ~HexSynth() override {}
  apvts* const linkedTree;
  void setSampleRate(double newRate, int blockSize = 512) {
    setCurrentPlaybackSampleRate(newRate);
    for (auto voice : hexVoices) {
      voice->setSampleRate(newRate, blockSize);
    }
  }
  juce::SynthesiserVoice* findFreeVoice(
      juce::SynthesiserSound* soundToPlay,
      int midiChannel,
      int midiNoteNum,
      bool stealIfNoneAvailible) const override;
  void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;
  void noteOff(int midiChannel,
               int midiNoteNumber,
               float velocity,
               bool allowTailOff) override;
  void renderVoices(juce::AudioBuffer<float>& buffer,
                    int startSample,
                    int numSamples) override;

  //===============================================
  void updateRoutingForBlock();
  void updateEnvelopesForBlock();
  void updateOscillatorsForBlock();
  void updateFiltersForBlock();
  void updateLfosForBlock();
  //! LFO update functions
  void setRate(int idx, float value);
  void setDepth(int idx, float value);
  void setLfoWave(int idx, float value);
  void setTarget(int idx, float value);
  //=================================================
  //! Filter Parameters
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
  void prepareRingBuffer(int blockSize) {
    juce::ignoreUnused(blockSize);
    graphBuffer.setSize(2, RING_BUFFER_READ_SIZE * 10);
  }
  void prepareVoiceBuffers(int blockSize) {
    for (auto v : hexVoices) {
      v->prepareBuffer(blockSize);
    }
  }
  GraphParamSet graphParams;
  RingBuffer<float> graphBuffer;

private:
  RoutingGrid grid;
  EnvelopeLUTGroup envelopeData;
  std::vector<HexVoice*> hexVoices;
  AsyncDebugPrinter printer;
  float magnitude;
  float lastMagnitude;
  long numJumps;
};
