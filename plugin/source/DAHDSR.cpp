
#include "HexHeader.h"
#include "Identifiers.h"
#include "MathUtil.h"
#include "DAHDSR.h"

namespace VelTracking {
static std::atomic<float> trackingAmt;

void setTrackingAmount(float amt) {
  trackingAmt.store(amt);
}
static frange_t getGainRange() {
  auto minGain = juce::Decibels::decibelsToGain(-10.0f);
  auto midGain = juce::Decibels::decibelsToGain(-6.0f);
  frange_t range(minGain, 1.0f);
  range.setSkewForCentre(midGain);
  return range;
}

float gainForVelocity(float vel) {
  static frange_t nRange = getGainRange();
  float fullDepth = nRange.convertFrom0to1(vel);
  auto value = MathUtil::fLerp(1.0f, fullDepth, trackingAmt.load());
  DBG("Velocity of " + String(vel) + " has gain " + String(value));
  return value;
}

}  // namespace VelTracking
//=========================================================================
static size_t msToSamples(float ms) {
  return (size_t)((ms / 1000.0f) * (float)SampleRate::get());
}

void SharedEnvData::computeLUTs() {
  // 1. figure out the length in samples for each
  // section
  delaySamples = msToSamples(delayMs);
  attackSamples = msToSamples(attackMs);
  decaySamples = msToSamples(decayMs);
  holdSamples = msToSamples(holdMs);
  releaseSamples = msToSamples(releaseMs);

  // 2. calculate the attack LUT
  static const float midAtkGain = juce::Decibels::decibelsToGain(-6.0f);
  const float attackExp = std::log(midAtkGain) / std::log(0.5f);
  float dX = 1.0f / (float)attackSamples;
  float xPos = 0.0f;
  for (size_t i = 0; i < attackSamples; ++i) {
    attackLut[i] = std::powf(xPos, attackExp);
    xPos += dX;
  }
  //  3. calculate the decay curve
  dX = 1.0f / (float)decaySamples;
  const float decayHeight = 1.0f - sustainLevel;
  const float decayExp = attackExp;
  xPos = 1.0f;
  for (size_t i = 0; i < decaySamples; ++i) {
    decayLut[i] = sustainLevel + (std::powf(xPos, decayExp) * decayHeight);
    xPos -= dX;
  }
  // 4. same for the release curve
  const float releaseExp = decayExp;
  dX = 1.0f / (float)releaseSamples;
  xPos = 1.0f;
  for (size_t i = 0; i < releaseSamples; ++i) {
    releaseLut[i] = std::powf(xPos, releaseExp) * sustainLevel;
    xPos -= dX;
  }
}

size_t SharedEnvData::sampleIdxForRetrig(float level) const {
  size_t left = 0;
  size_t right = attackSamples - 1;
  while (left <= right) {
    auto mid = left + (right - left) / 2;
    if (attackLut[mid] <= level && attackLut[mid + 1] > level) {
      const float diff1 = std::fabs(level - attackLut[mid]);
      const float diff2 = std::fabs(level - attackLut[mid + 1]);
      return (diff1 < diff2) ? mid : mid + 1;
    } else if (attackLut[mid] < level) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  jassert(false);
  return 0;
}

float SharedEnvData::nextValue(EnvPhase& phase, size_t& samplesInPhase) const {
  ++samplesInPhase;
  switch (phase) {
    case delayPhase:
      if (samplesInPhase >= delaySamples) {
        phase = attackPhase;
        samplesInPhase = 0;
      }
      return 0.0f;
    case attackPhase:
      if (samplesInPhase >= attackSamples) {
        phase = holdPhase;
        samplesInPhase = 0;
        return 1.0f;
      }
      return attackLut[samplesInPhase];
    case holdPhase:
      if (samplesInPhase >= holdSamples) {
        phase = decayPhase;
        samplesInPhase = 0;
      }
      return 1.0f;
    case decayPhase:
      if (samplesInPhase >= decaySamples) {
        phase = sustainPhase;
        samplesInPhase = 0;
        return sustainLevel;
      }
      return decayLut[samplesInPhase];
    case sustainPhase:
      return sustainLevel;
    case releasePhase:
      if (samplesInPhase >= releaseSamples) {
        phase = noteOff;
        samplesInPhase = 0;
        return 0.0f;
      }
      return releaseLut[samplesInPhase];
    case noteOff:
      return 0.0f;
  }
  jassert(false);
  return 0.0f;
}

void SharedEnvData::setDelay(float delay) {
  if (!fequal(delay, delayMs)) {
    delayMs = delay;
    triggerAsyncUpdate();
  }
}

void SharedEnvData::setAttack(float attack) {
  if (!fequal(attack, attackMs)) {
    attackMs = attack;
    triggerAsyncUpdate();
  }
}

void SharedEnvData::setHold(float hold) {
  if (!fequal(hold, holdMs)) {
    holdMs = hold;
    triggerAsyncUpdate();
  }
}

void SharedEnvData::setDecay(float decay) {
  if (!fequal(decay, decayMs)) {
    decayMs = decay;
    triggerAsyncUpdate();
  }
}

void SharedEnvData::setSustain(float level) {
  if (!fequal(level, sustainLevel)) {
    sustainLevel = level;
    triggerAsyncUpdate();
  }
}

void SharedEnvData::setRelease(float release) {
  if (!fequal(release, releaseMs)) {
    releaseMs = release;
    triggerAsyncUpdate();
  }
}

SharedEnvData::SharedEnvData() {
  computeLUTs();
}
//=========================================================================

VoiceEnvelope::VoiceEnvelope(SharedEnvData* d) : envData(d) {}

void VoiceEnvelope::triggerOn(float velocity) {
  vGain = VelTracking::gainForVelocity(velocity);
  if (currentPhase != noteOff) {
    currentPhase = attackPhase;
    sampleIdx = envData->sampleIdxForRetrig(lastLevel);
  } else {
    currentPhase = delayPhase;
    sampleIdx = 0;
  }
}

void VoiceEnvelope::triggerOff() {
  currentPhase = releasePhase;
  sampleIdx = 0;
}

float VoiceEnvelope::process(float input) {
  lastLevel = envData->nextValue(currentPhase, sampleIdx) * vGain;
  return input * lastLevel;
}

//=========================================================================
void DAHDSR::enterPhase(EnvPhase newPhase) {
  currentPhase = newPhase;
  samplesIntoPhase = 0;
  switch (newPhase) {
    case delayPhase: {
      _startLevel = minLevel;
      _endLevel = minLevel;

      samplesInPhase = (size_t)((double)delayTime * sampleRate / 1000.0);
      factor = factorFor(_startLevel, _endLevel, delayTime);
      break;
    }
    case attackPhase: {
      _startLevel = minLevel;
      _endLevel = 1.0f;
      samplesInPhase = (size_t)((double)attackTime * sampleRate / 1000.0);
      factor = factorFor(_startLevel, _endLevel, attackTime);
      break;
    }
    case holdPhase: {
      _startLevel = 1.0f;
      _endLevel = 1.0f;
      samplesInPhase = (size_t)((double)holdTime * sampleRate / 1000.0);
      factor = factorFor(_startLevel, _endLevel, holdTime);
      break;
    }
    case decayPhase: {
      _startLevel = 1.0f;
      _endLevel = sustainLevel;
      samplesInPhase = (size_t)((double)decayTime * sampleRate / 1000.0);
      factor = factorFor(_startLevel, _endLevel, decayTime);
      break;
    }
    case sustainPhase: {
      _startLevel = sustainLevel;
      _endLevel = sustainLevel;
      samplesInPhase = 1000000;
      factor = 1.0f;
      break;
    }
    case releasePhase: {
      _startLevel = sustainLevel;
      _endLevel = minLevel;
      samplesInPhase = (size_t)((double)releaseTime * sampleRate / 1000.0);
      factor = factorFor(_startLevel, _endLevel, releaseTime);
      break;
    }
    case noteOff: {
      _startLevel = minLevel;
      _endLevel = minLevel;
      samplesInPhase = 100000000;
      factor = 0.0f;
      break;
    }
  }
  output = _startLevel;
  updatePhase();
}
float DAHDSR::process(float input) {
  lastOutput = output;
  updatePhase();
  ++samplesIntoPhase;
  output *= (float)factor;
  return input * output * vGain;
}

void DAHDSR::killQuick(float msFade) {
  //! function to bring down the envelope output very quickly (but not
  //! immediately) for voice stealing
  currentPhase = releasePhase;
  _startLevel = output;
  _endLevel = minLevel;
  samplesInPhase = (size_t)((double)msFade * sampleRate / 1000.0);
  factor = factorFor(_startLevel, _endLevel, msFade);
}

void DAHDSR::printDebug() {
  printf("This output: %f\n", output);
  printf("Last output: %f\n", lastOutput);
}
