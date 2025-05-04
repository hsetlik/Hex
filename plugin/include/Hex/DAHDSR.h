/*
  ==============================================================================

    DAHDSR.h
    Created: 6 Oct 2020 1:07:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#define DELAY_MIN 0.0f
#define DELAY_MAX 8000.0f
#define DELAY_DEFAULT 0.0f
#define DELAY_CENTER 1000.0f

#define ATTACK_MIN 0.0f
#define ATTACK_MAX 8000.0f
#define ATTACK_DEFAULT 20.0f
#define ATTACK_CENTER 1000.0f

#define HOLD_MIN 0.0f
#define HOLD_MAX 8000.0f
#define HOLD_DEFAULT 0.0f
#define HOLD_CENTER 1000.0f

#define DECAY_MIN 0.0f
#define DECAY_MAX 8000.0f
#define DECAY_DEFAULT 40.0f
#define DECAY_CENTER 1000.0f

#define SUSTAIN_MIN 0.0f
#define SUSTAIN_MAX 1.0f
#define SUSTAIN_DEFAULT 0.6f

#define RELEASE_MIN 0.0f
#define RELEASE_MAX 4000.0f
#define RELEASE_DEFAULT 80.0f
#define RELEASE_CENTER 1000.0f

class DAHDSR {
public:
  enum envPhase {
    retrigPhase,
    delayPhase,
    attackPhase,
    holdPhase,
    decayPhase,
    sustainPhase,
    releasePhase,
    noteOff
  };
  // functions
  DAHDSR(int ind = 0)
      : lastOutput(0.0f), factor(1.0f), sampleRate(44100), index(ind) {
    trigger = false;
    samplesIntoPhase = 0;
    currentPhase = noteOff;
  }
  ~DAHDSR() {}
  static envPhase nextPhase(envPhase input) {
    if (input != noteOff)
      return (envPhase)(input + 1);
    else
      return noteOff;
  }
  void triggerOn() {
    trigger = true;
    if (currentPhase != noteOff)
      enterPhase(retrigPhase);
    else
      enterPhase(delayPhase);
  }
  float factorFor(float startLevel, float endLevel, float lengthMs) {
    if (startLevel == 0.0f)
      startLevel = minLevel;
    if (endLevel == 0.0f)
      endLevel = minLevel;
    unsigned long phaseLengthSamples =
        (unsigned long)(lengthMs * (float)(sampleRate / 1000.0));
    return exp((log(endLevel) - log(startLevel)) / (float)phaseLengthSamples);
  }
  void triggerOff() {
    trigger = false;
    enterPhase(releasePhase);
  }
  void updatePhase() {
    if (samplesIntoPhase > samplesInPhase || samplesInPhase < 1) {
      enterPhase(nextPhase(currentPhase));
    }
  }
  void enterPhase(envPhase newPhase);
  void killQuick(float msFade = 10.0f);
  void setSampleRate(double value) { sampleRate = value; }
  float process(float input);
  float clockOutput() { return process(1.0f); }
  bool isActive() { return !(currentPhase == noteOff); }
  envPhase getPhase() { return currentPhase; }
  float output;
  float lastOutput;
  void setDelay(float val) { delayTime = val; }
  void setAttack(float val) { attackTime = val; }
  void setHold(float val) { holdTime = val; }
  void setDecay(float val) { decayTime = val; }
  void setSustain(float val) { sustainLevel = val; }
  void setRelease(float val) { releaseTime = val; }
  float getLastLevel() const { return output; }
  void printDebug();

private:
  // data
  envPhase currentPhase;
  unsigned long long samplesIntoPhase;
  unsigned long long samplesInPhase;
  double factor;
  float minLevel = 0.00001f;
  double sampleRate;
  int index;
  bool trigger;
  float delayTime = DELAY_DEFAULT;
  float attackTime = ATTACK_DEFAULT;
  float holdTime = HOLD_DEFAULT;
  float decayTime = DECAY_DEFAULT;
  float sustainLevel = SUSTAIN_DEFAULT;
  float releaseTime = RELEASE_DEFAULT;
  float _startLevel;
  float _endLevel;
};
