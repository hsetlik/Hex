/*
  ==============================================================================

    Filter.h
    Created: 14 Jun 2021 10:59:58am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <cstdint>
#include "DAHDSR.h"
#include "MathUtil.h"
#include "juce_core/juce_core.h"
#define CUTOFF_MIN 20.0f
#define CUTOFF_MAX 20000.0f
#define CUTOFF_DEFAULT 4000.0f
#define CUTOFF_CENTER 100.0f

#define RESONANCE_MIN 0.1f
#define RESONANCE_MAX 35.0f
#define RESONANCE_DEFAULT 1.0f
#define RESONANCE_CENTER 5.0f
enum FilterType { LoPass, HiPass, BandPass };

class FilterCore {
public:
  FilterCore()
      : sampleRate(44100.0f), cutoff(2500.0f), resonance(1.0f), blockSize(512) {
    setup();
  }
  virtual ~FilterCore() {}
  virtual float process(float input) {
    juce::ignoreUnused(input);
    return 0.0f;
  }
  virtual float processWithMod(float input, float modValue) {
    setup(modValue);
    return process(input);
  }
  virtual void setup() {}
  virtual void setup(float modValue) { juce::ignoreUnused(modValue); }
  virtual void setResonance(float value) {
    resonance = value;
    setup();
  }
  virtual void setCutoff(float value) {
    cutoff = value;
    setup();
  }
  virtual void setSampleRate(double rate, int block = 512) {
    sampleRate = rate;
    blockSize = block;
    setup();
  }

protected:
  double sampleRate;
  float cutoff;
  float resonance;
  int blockSize;
};

class LibLowPass : public FilterCore {
public:
  void setup() override {
    // filter.setup (sampleRate, (double)cutoff, (double)resonance);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32_t)blockSize;
    spec.numChannels = 1;
    jFilter.prepare(spec);
    jFilter.setCutoffFrequency(cutoff);
    jFilter.setResonance(resonance);
    jFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
  }
  void setup(float modValue) override {
    // filter.setup (sampleRate, (double)cutoff + ((CUTOFF_MAX - cutoff) *
    // modValue), (double)resonance);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32_t)blockSize;
    spec.numChannels = 1;
    jFilter.prepare(spec);
    jFilter.setCutoffFrequency(cutoff + ((CUTOFF_MAX - cutoff) * modValue));
    jFilter.setResonance(resonance);
    jFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
  }
  float process(float input) override {
    return jFilter.processSample(0, input);
  }

private:
  // Iir::RBJ::LowPass filter;
  juce::dsp::StateVariableTPTFilter<float> jFilter;
};

class LibHiPass : public FilterCore {
public:
  void setup() override {
    // filter.setup (sampleRate, (double)cutoff, (double)resonance);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32_t)blockSize;
    spec.numChannels = 1;
    jFilter.prepare(spec);
    jFilter.setCutoffFrequency(cutoff);
    jFilter.setResonance(resonance);
    jFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
  }
  float process(float input) override {
    return jFilter.processSample(0, input);
  }
  void setup(float modValue) override {
    // filter.setup (sampleRate, (double)cutoff + ((CUTOFF_MAX - cutoff) *
    // modValue), (double)resonance);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32_t)blockSize;
    spec.numChannels = 1;
    jFilter.prepare(spec);
    jFilter.setCutoffFrequency(cutoff + ((CUTOFF_MAX - cutoff) * modValue));
    jFilter.setResonance(resonance);
    jFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
  }

private:
  // Iir::RBJ::HighPass filter;
  juce::dsp::StateVariableTPTFilter<float> jFilter;
};

class LibBandPass : public FilterCore {
public:
  void setup() override {
    // filter.setup (sampleRate, (double)cutoff, (double)resonance);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32_t)blockSize;
    spec.numChannels = 1;
    jFilter.prepare(spec);
    jFilter.setCutoffFrequency(cutoff);
    jFilter.setResonance(resonance);
    jFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
  }

  void setup(float modValue) override {
    // filter.setup (sampleRate, (double)cutoff + ((CUTOFF_MAX - cutoff) *
    // modValue), (double)resonance);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32_t)blockSize;
    spec.numChannels = 1;
    jFilter.prepare(spec);
    jFilter.setCutoffFrequency(cutoff + ((CUTOFF_MAX - cutoff) * modValue));
    jFilter.setResonance(resonance);
    jFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
  }

  float process(float input) override {
    return jFilter.processSample(0, input);
  }

private:
  // Iir::RBJ::BandPass1 filter;
  juce::dsp::StateVariableTPTFilter<float> jFilter;
};

class StereoFilter : public juce::AsyncUpdater {
public:
  StereoFilter(EnvelopeLUTGroup* luts, int voiceIdx);
  void setSampleRate(double rate, int blockSize = 512) {
    juce::ignoreUnused(blockSize);
    rateVal = rate;
    lFilter->setSampleRate(rateVal);
    rFilter->setSampleRate(rateVal);
  }
  void setCutoff(float value) {
    cutoffVal = value;
    lFilter->setCutoff(cutoffVal);
    rFilter->setCutoff(cutoffVal);
  }
  void setResonance(float value) {
    resonanceVal = value;
    lFilter->setResonance(resonanceVal);
    rFilter->setResonance(resonanceVal);
  }
  void setDepth(float value) { envDepth = value; }
  void setWetLevel(float value) { wetLevel = value; }
  void tick() {
    auto modVal = env.process(envDepth);
    auto inc = (CUTOFF_MAX - cutoffVal) * modVal;
    lFilter->setCutoff(cutoffVal + inc);
    rFilter->setCutoff(cutoffVal + inc);
  }
  void handleAsyncUpdate() override;
  void setType(int filterType);
  float processLeft(float input) {
    return MathUtil::fLerp(input, lFilter->process(input), wetLevel);
  }
  float processLeft(float input, float modValue) {
    return MathUtil::fLerp(input, lFilter->processWithMod(input, modValue),
                           wetLevel);
  }
  float processRight(float input) {
    return MathUtil::fLerp(input, rFilter->process(input), wetLevel);
  }
  float processRight(float input, float modValue) {
    return MathUtil::fLerp(input, rFilter->processWithMod(input, modValue),
                           wetLevel);
  }
  VoiceEnvelope env;
  float getCutoff() const { return cutoffVal; }

private:
  float cutoffVal;
  float resonanceVal;
  double rateVal;
  float envDepth;
  float wetLevel;
  const int voiceIndex;
  FilterType currentType;
  std::unique_ptr<FilterCore> lFilter;
  std::unique_ptr<FilterCore> rFilter;
};
