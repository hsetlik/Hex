/*
  ==============================================================================

    LFO.h
    Created: 22 Jun 2021 2:13:30pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "MathUtil.h"
#include "FMOscillator.h"
#define RATE_MIN 0.01f
#define RATE_MAX 20.0f
#define RATE_DEFAULT 1.0f
#define RATE_CENTER 2.0f

typedef std::array<float, TABLESIZE> LfoArray;

class WaveArray {
public:
  static LfoArray arrayForType(WaveType type);
};

class LfoBase {
public:
  LfoBase() : rate(1.0f), sampleRate(44100.0f) {}
  virtual ~LfoBase() {}
  virtual void setSampleRate(double sr) { sampleRate = sr; }
  virtual void setRate(float speedHz) { rate = speedHz; }
  virtual float tick() { return 0.0f; }

protected:
  float rate;
  double sampleRate;
};

class WaveLfo {
public:
  WaveLfo(WaveType type) : data(WaveArray::arrayForType(type)), phase(0.0f) {}
  float tick() {
    phaseDelta = rate / (float)sampleRate;
    phase += phaseDelta;
    if (phase > 1.0f)
      phase -= 1.0f;
    lowerIdx = (int)std::floor(phase * (float)TABLESIZE);
    upperIdx = (lowerIdx == TABLESIZE - 1) ? 0 : lowerIdx + 1;
    skew = (phase * (float)TABLESIZE) - (float)lowerIdx;
    return MathUtil::fLerp(data[(size_t)lowerIdx], data[(size_t)upperIdx],
                           skew);
  }
  void setRate(float speedHz) { rate = speedHz; }
  void setSampleRate(double sr) { sampleRate = sr; }

private:
  float rate;
  double sampleRate;
  LfoArray data;
  float phase;
  float phaseDelta;
  int lowerIdx;
  int upperIdx;
  float skew;
};

class NoiseLfo {
public:
  NoiseLfo() : phase(0.0f), rGen(12) { output = rGen.nextFloat(); }
  float tick() {
    phaseDelta = rate / (float)sampleRate;
    phase += phaseDelta;
    if (phase > 1.0f) {
      phase -= 1.0f;
      output = rGen.nextFloat();
    }
    return output;
  }
  void setRate(float speedHz) { rate = speedHz; }
  void setSampleRate(double sr) { sampleRate = sr; }

private:
  float rate;
  double sampleRate;
  float phase;
  float phaseDelta;
  juce::Random rGen;
  float output;
};

class HexLfo : public juce::AsyncUpdater {
public:
  HexLfo(int idx);
  const int lfoIndex;
  float tick();
  float tickToValue(float baseValue, float maxValue, float depth);
  void setSampleRate(double rate);
  void setRate(float rate);
  void setType(int type);
  void handleAsyncUpdate() override;

private:
  WaveType currentType;
  std::unique_ptr<LfoBase> pOsc;
  std::unique_ptr<WaveLfo> waveOsc;
  NoiseLfo noiseOsc;
  double sampleRate;
  float currentRate = 0.0f;
};
