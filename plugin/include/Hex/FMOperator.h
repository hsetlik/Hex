/*
  ==============================================================================

    FMOperator.h
    Created: 4 Jun 2021 2:15:51pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "FMOscillator.h"
#include "DAHDSR.h"
#define NUM_VOICES 18
#define NUM_LFOS 4
//! macros for use in parameter layout
#define RATIO_MIN 0.1f
#define RATIO_MAX 10.0f
#define RATIO_DEFAULT 1.0f
#define RATIO_CENTER 1.0f

#define MODINDEX_MIN 0.0f
#define MODINDEX_MAX 450.0f
#define MODINDEX_DEFAULT 0.0f
#define MODINDEX_CENTER 150.0f

#define PAN_MIN 0.0f
#define PAN_MAX 1.0f
#define PAN_DEFAULT 0.5f

using RoutingGrid = std::array<std::array<bool, NUM_OPERATORS>, NUM_OPERATORS>;
class FMOperator {
public:
  FMOperator(int opIndex, EnvelopeLUTGroup* luts);
  void trigger(bool on, float velocity = 1.0f) {
    if (on)
      vEnv.triggerOn(velocity);
    else
      vEnv.triggerOff();
  }
  void setSampleRate(double rate);
  //! functions to set private variables
  void setRatio(float value) { baseRatio = value; }
  void setModIndex(float value) { modIndex = value; }
  void setPan(float value) { pan = value; }
  void setLevel(float value) { level = value; }
  void setAudible(bool shouldBeAudible) { audible = shouldBeAudible; }
  void clearOffset() { modOffset = 0.0f; }
  float getLevel() const { return level; }

private:
  bool audible;

public:
  const int index;
  //! access to variables
  bool isAudible() const { return audible; }
  float lastMono() const { return lastOutMono; }
  float lastLeft() const { return lastOutL; }
  float lastRight() const { return lastOutR; }
  //! where the magic happens
  void addModFrom(const FMOperator& source) { modOffset += source.lastMono(); }
  void tick(double fundamental);
  void tick(double fundamental, float modValue);
  void setWave(int type) { oscillator.setType((WaveType)type); }
  HexOsc oscillator;
  VoiceEnvelope vEnv;

private:
  //! Settable parameters stored down here
  float modIndex;
  float baseRatio;
  float modOffset;
  float pan;
  float level;
  //!  output variables
  float lastOutMono;
  float lastOutL;
  float lastOutR;
};
