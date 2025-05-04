/*
  ==============================================================================

    FMOscillator.h
    Created: 4 Jun 2021 1:24:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define TABLES_PER_FRAME 10
#define TABLESIZE 1024

enum WaveType { Sine, Square, Saw, Tri, Noise };

struct Wavetable {
  float table[TABLESIZE];
  float minFreq;
  float maxFreq;
};

namespace WTArray {
std::array<float, TABLESIZE> makeArray(WaveType type);
}

class OscBase {
public:
  OscBase() : sampleRate(44100.0f), nyquist(sampleRate / 2.0f) {}
  virtual ~OscBase() {}
  virtual void setSampleRate(double rate) {
    sampleRate = rate;
    nyquist = sampleRate / 2.0f;
  }
  virtual float getSample(double hz) {
    juce::ignoreUnused(hz);
    return 0.0f;
  }

protected:
  double sampleRate;
  double nyquist;
};

class SineOsc {
public:
  SineOsc();
  float getSample(double hz);
  void setSampleRate(double rate) {
    sampleRate = rate;
    nyquist = sampleRate / 2.0f;
  }

private:
  double sampleRate;
  double nyquist;
  float phase;
  float phaseDelta;
  float skew;
  int lowerIdx;
  int upperIdx;
  float sineData[TABLESIZE];
};

class AntiAliasOsc {
public:
  AntiAliasOsc(WaveType type = WaveType::Square);
  float getSample(double hz);
  void createTables(int size, float* real, float* imag);
  float makeTable(float* waveReal,
                  float* waveImag,
                  int numSamples,
                  float scale,
                  float bottomFreq,
                  float topFreq);
  Wavetable* tableForHz(double hz);
  void setSampleRate(double rate) {
    sampleRate = rate;
    nyquist = sampleRate / 2.0f;
  }

private:
  double sampleRate = 44100.0;
  double nyquist = sampleRate / 2.0;
  juce::OwnedArray<Wavetable> tables;
  float phase;
  float phaseDelta;
  int tablesAdded;
  int bottomIndex;
  float bSample;
  float tSample;
  float skew;
};

class NoiseOsc {
public:
  NoiseOsc() : rGen(2341) {}
  float getSample(double) { return (rGen.nextFloat() * 2.0f) - 1.0f; }
  void setSampleRate(double rate) {
    sampleRate = rate;
    nyquist = sampleRate / 2.0f;
  }

private:
  double sampleRate = 44100.0;
  double nyquist = sampleRate / 2.0;

  juce::Random rGen;
};

class HexOsc : public juce::AsyncUpdater {
private:
  enum OscModeE { mSine, mWave, mNoise };
  OscModeE oMode = OscModeE::mSine;

public:
  HexOsc();
  void handleAsyncUpdate() override;
  void setType(WaveType type);
  void setSampleRate(double rate);
  float getSample(double hz);

private:
  WaveType currentType;
  SineOsc sineOsc;
  std::unique_ptr<AntiAliasOsc> waveOsc;
  NoiseOsc nOsc;
};
