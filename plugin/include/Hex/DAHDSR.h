
#pragma once
#include <JuceHeader.h>
#include "juce_events/juce_events.h"

#define DELAY_MIN 0.0f
#define DELAY_MAX 4000.0f
#define DELAY_DEFAULT 0.0f
#define DELAY_CENTER 1000.0f

#define ATTACK_MIN 0.0f
#define ATTACK_MAX 4000.0f
#define ATTACK_DEFAULT 20.0f
#define ATTACK_CENTER 1000.0f

#define HOLD_MIN 0.0f
#define HOLD_MAX 8000.0f
#define HOLD_DEFAULT 0.0f
#define HOLD_CENTER 1000.0f

#define DECAY_MIN 0.0f
#define DECAY_MAX 4000.0f
#define DECAY_DEFAULT 40.0f
#define DECAY_CENTER 1000.0f

#define SUSTAIN_MIN 0.0f
#define SUSTAIN_MAX 1.0f
#define SUSTAIN_DEFAULT 0.6f

#define RELEASE_MIN 0.0f
#define RELEASE_MAX 4000.0f
#define RELEASE_DEFAULT 80.0f
#define RELEASE_CENTER 1000.0f
#define MAX_SAMPLE_RATE 96000.0f

enum EnvPhase {
  delayPhase,
  attackPhase,
  holdPhase,
  decayPhase,
  sustainPhase,
  releasePhase,
  noteOff
};

namespace VelTracking {
void setTrackingAmount(float amt);
float gainForVelocity(float vel);
}  // namespace VelTracking

constexpr size_t MAX_LUT_SIZE =
    (size_t)((RELEASE_MAX / 1000.0f) * MAX_SAMPLE_RATE);

typedef std::array<float, MAX_LUT_SIZE> lut_array_t;

// this object should only be instantiated once per operator,
// voices will need a pointer to it
class SharedEnvData : public juce::AsyncUpdater {
private:
  juce::CriticalSection critSection;
  float delayMs = DELAY_DEFAULT;
  float attackMs = ATTACK_DEFAULT;
  float holdMs = HOLD_DEFAULT;
  float decayMs = DECAY_DEFAULT;
  float sustainLevel = SUSTAIN_DEFAULT;
  float releaseMs = RELEASE_DEFAULT;

  size_t delaySamples;
  size_t attackSamples;
  size_t holdSamples;
  size_t decaySamples;
  size_t releaseSamples;

  lut_array_t attackLut;
  lut_array_t decayLut;
  lut_array_t releaseLut;

  void computeLUTs();

public:
  SharedEnvData();
  // param setters
  void setDelay(float delay);
  void setAttack(float attack);
  void setHold(float hold);
  void setDecay(float decay);
  void setSustain(float lvl);
  void setRelease(float release);
  size_t sampleIdxForRetrig(float level) const;
  // notice that this takes references because it handles updating for
  // the per-voice objects
  float nextValue(EnvPhase& phase, size_t& samplesInPhase) const;
  void handleAsyncUpdate() override { computeLUTs(); }
};

// the per-voice objects for the envelope implementations
class VoiceEnvelope {
private:
  SharedEnvData* const envData;
  EnvPhase currentPhase = EnvPhase::noteOff;
  size_t sampleIdx = 0;
  float vGain = 1.0f;
  float lastLevel = 0.0f;

public:
  VoiceEnvelope(SharedEnvData* d);
  void triggerOn(float velocity);
  void triggerOff();
  float process(float input);
  float getLastLevel() const { return lastLevel; }
  bool isActive() const { return !(currentPhase == noteOff); }
};

//===============================================================
class DAHDSR {
public:
  // functions
  DAHDSR(int ind = 0)
      : lastOutput(0.0f), factor(1.0f), sampleRate(44100), index(ind) {
    trigger = false;
    samplesIntoPhase = 0;
    currentPhase = noteOff;
  }
  ~DAHDSR() {}
  static EnvPhase nextPhase(EnvPhase input) {
    if (input != noteOff)
      return (EnvPhase)(input + 1);
    else
      return noteOff;
  }
  void triggerOn(float velocity) {
    trigger = true;
    vGain = VelTracking::gainForVelocity(velocity);
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
  void enterPhase(EnvPhase newPhase);
  void killQuick(float msFade = 10.0f);
  void setSampleRate(double value) { sampleRate = value; }
  float process(float input);
  bool isActive() { return !(currentPhase == noteOff); }
  EnvPhase getPhase() { return currentPhase; }
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
  EnvPhase currentPhase;
  size_t samplesIntoPhase;
  size_t samplesInPhase;
  double factor;
  const float minLevel = 0.00001f;
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
  float vGain = 1.0f;
};
