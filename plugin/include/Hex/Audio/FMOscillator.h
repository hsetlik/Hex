/*
  ==============================================================================

    FMOscillator.h
    Created: 4 Jun 2021 1:24:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "HexHeader.h"
#define TABLES_PER_FRAME 10
#define TABLESIZE 2048

struct Wavetable {
  float table[TABLESIZE];
  float minFreq;
  float maxFreq;
};

namespace WTArray {
std::array<float, TABLESIZE> makeArray(WaveType type);
}

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
