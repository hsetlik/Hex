#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"

typedef juce::AudioParameterFloat AudioParamFloat;
typedef juce::AudioParameterChoice AudioParamChoice;
typedef juce::AudioParameterBool AudioParamBool;
typedef juce::NormalisableRange<float> frange_t;

#define DECLARE_ID(name) const juce::Identifier name(#name);
namespace ID {
// top level tree type
DECLARE_ID(HEX_STATE_TREE)
// global params-----------------------
DECLARE_ID(velocityTracking)
DECLARE_ID(useSustainPedal)
// Filter params ----------------------
DECLARE_ID(filterEnvDelay)
DECLARE_ID(filterEnvAttack)
DECLARE_ID(filterEnvHold)
DECLARE_ID(filterEnvDecay)
DECLARE_ID(filterEnvSustain)
DECLARE_ID(filterEnvRelease)

DECLARE_ID(filterType)
DECLARE_ID(filterCutoff)
DECLARE_ID(filterResonance)
DECLARE_ID(filterWetDry)
DECLARE_ID(filterEnvDepth)

// LFO params-----------------------
DECLARE_ID(lfoRate)
DECLARE_ID(lfoDepth)
DECLARE_ID(lfoSync)
DECLARE_ID(lfoWave)
DECLARE_ID(lfoTarget)

// Oscillator/modulation------------
DECLARE_ID(operatorRatio)
DECLARE_ID(operatorLevel)
DECLARE_ID(operatorAudible)
DECLARE_ID(operatorModIndex)
DECLARE_ID(operatorPan)
DECLARE_ID(operatorWaveShape)

// Operator envelopes--------------
DECLARE_ID(envDelay)
DECLARE_ID(envAttack)
DECLARE_ID(envHold)
DECLARE_ID(envDecay)
DECLARE_ID(envSustain)
DECLARE_ID(envRelease)
// child tree that contains the patch info------
DECLARE_ID(HEX_PATCH_INFO)
DECLARE_ID(patchName)
DECLARE_ID(patchAuthor)
DECLARE_ID(patchType)
}  // namespace ID
#undef DECLARE_ID
