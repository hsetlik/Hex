/*
  ==============================================================================

    LFO.cpp
    Created: 22 Jun 2021 2:13:30pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LFO.h"

LfoArray WaveArray::arrayForType(WaveType type) {
  LfoArray array;
  switch (type) {
    case Sine: {
      auto dPhase = juce::MathConstants<float>::twoPi / (float)TABLESIZE;
      for (size_t i = 0; i < TABLESIZE; ++i) {
        array[i] = (std::sin(dPhase * (float)i) / 2.0f) + 0.5f;
      }
      break;
    }
    case Square: {
      for (size_t i = 0; i < TABLESIZE; ++i) {
        if (i < TABLESIZE / 2)
          array[i] = 1.0f;
        else
          array[i] = 0.0f;
      }
      break;
    }
    case Saw: {
      auto dY = 1.0f / (float)TABLESIZE;
      for (size_t i = 0; i < TABLESIZE; ++i) {
        array[i] = (float)i * dY;
      }
      break;
    }
    case Tri: {
      auto dY = 2.0f / TABLESIZE;
      float level = 0.0f;
      for (size_t i = 0; i < TABLESIZE; ++i) {
        if (i < TABLESIZE / 2)
          level += dY;
        else
          level -= dY;
        array[i] = level;
      }
      break;
    }
    case Noise: {
      break;
    }
  }
  return array;
}
//====================================================================================
HexLfo::HexLfo(int idx)
    : lfoIndex(idx),
      currentType(Sine),
      waveOsc(std::make_unique<WaveLfo>(currentType)),
      sampleRate(44100.0f),
      currentRate(1.0f) {}
void HexLfo::setType(int type) {
  //! type changes
  if (type != (int)currentType) {
    currentType = (WaveType)type;
    triggerAsyncUpdate();
  }
}

void HexLfo::handleAsyncUpdate() {
  if (currentType != Noise)
    waveOsc.reset(new WaveLfo(currentType));
  //! Make sure other parameters stay the same when the type gets changed
  waveOsc->setSampleRate(sampleRate);
  waveOsc->setRate(currentRate);
}

void HexLfo::setRate(float _rate) {
  waveOsc->setRate(_rate);
}

void HexLfo::setSampleRate(double rate) {
  sampleRate = rate;
  waveOsc->setSampleRate(rate);
  noiseOsc.setSampleRate(rate);
}
float HexLfo::tick() {
  if (currentType != Noise) {
    return waveOsc->tick();
  } else {
    return noiseOsc.tick();
  }
}
float HexLfo::tickToValue(float baseValue, float maxValue, float depth) {
  return MathUtil::fLerp(baseValue, maxValue, tick() * depth);
}
