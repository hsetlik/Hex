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

class WTArray {
public:
  static std::array<float, TABLESIZE> makeArray(WaveType type);
};

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

class SineOsc : public OscBase {
public:
  SineOsc();
  float getSample(double hz) override;

private:
  float phase;
  float phaseDelta;
  float skew;
  int lowerIdx;
  int upperIdx;
  float sineData[TABLESIZE];
};

class AntiAliasOsc : public OscBase {
public:
  AntiAliasOsc(WaveType type);
  float getSample(double hz) override;
  void createTables(int size, float* real, float* imag);
  float makeTable(float* waveReal,
                  float* waveImag,
                  int numSamples,
                  float scale,
                  float bottomFreq,
                  float topFreq);
  Wavetable* tableForHz(double hz);

private:
  juce::OwnedArray<Wavetable> tables;
  float phase;
  float phaseDelta;
  int tablesAdded;
  int bottomIndex;
  float bSample;
  float tSample;
  float skew;
};

class NoiseOsc : public OscBase {
public:
  NoiseOsc() : rGen(2341) {}
  float getSample(double) override { return (rGen.nextFloat() * 2.0f) - 1.0f; }

private:
  juce::Random rGen;
};

class HexOsc : public juce::AsyncUpdater {
public:
  HexOsc();
  void handleAsyncUpdate() override;
  void setType(WaveType type);
  void setSampleRate(double rate) { pOsc->setSampleRate(rate); }
  float getSample(double hz) { return pOsc->getSample(hz); }

private:
  std::unique_ptr<OscBase> pOsc;
  WaveType currentType;
};
