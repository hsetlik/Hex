/*
  ==============================================================================

    ParameterLayout.h
    Created: 7 Jun 2021 1:32:49pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "DAHDSR.h"
#include "FMOperator.h"
#include "Filter.h"
#include "LfoComponent.h"
#include "LFO.h"
#include "Identifiers.h"
#include "juce_audio_processors/juce_audio_processors.h"
using fRange = juce::NormalisableRange<float>;

class HexParameters {
public:
  static apvts::ParameterLayout createLayout() {
    apvts::ParameterLayout layout;
    String vTrackID = ID::velocityTracking.toString();
    String vTrackName = "Velocity sensitivity";
    layout.add(std::make_unique<AudioParamFloat>(juce::ParameterID{vTrackID, 1},
                                                 vTrackName, 0.0f, 1.0f, 1.0f));
    String susPedalID = ID::useSustainPedal.toString();
    String susPedalName = "Use sustain pedal";
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{susPedalID, 1}, susPedalName, false));

    // static const int minPatchIdx = -1;
    // static const int maxPatchIdx = 512;
    // juce::ParameterID patchNumID{ID::selectedPatchIndex.toString(), 1};
    // String patchNumName = "Selected patch";
    // layout.add(std::make_unique<juce::AudioParameterInt>(
    //     patchNumID, patchNumName, minPatchIdx, maxPatchIdx, minPatchIdx));
    auto delName = "Filter Delay";
    auto aName = "Filter Attack";
    auto hName = "Filter Hold";
    auto decName = "Filter Decay";
    auto susName = "Filter Sustain";
    auto relName = "Filter Release";
    fRange delayRange(DELAY_MIN, DELAY_MAX);
    delayRange.setSkewForCentre(DELAY_CENTER);
    fRange attackRange(ATTACK_MIN, ATTACK_MAX);
    attackRange.setSkewForCentre(ATTACK_CENTER);
    fRange holdRange(HOLD_MIN, HOLD_MAX);
    holdRange.setSkewForCentre(HOLD_CENTER);
    fRange decayRange(DECAY_MIN, DECAY_MAX);
    decayRange.setSkewForCentre(DECAY_CENTER);
    fRange sustainRange(SUSTAIN_MIN, SUSTAIN_MAX);
    fRange releaseRange(RELEASE_MIN, RELEASE_MAX);
    releaseRange.setSkewForCentre(RELEASE_CENTER);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ID::filterEnvDelay.toString(), 1}, delName,
        delayRange, DELAY_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ID::filterEnvAttack.toString(), 1}, aName,
        attackRange, ATTACK_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ID::filterEnvHold.toString(), 1}, hName, holdRange,
        HOLD_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ID::filterEnvDecay.toString(), 1}, decName,
        decayRange, DECAY_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ID::filterEnvSustain.toString(), 1}, susName,
        sustainRange, SUSTAIN_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ID::filterEnvRelease.toString(), 1}, relName,
        releaseRange, RELEASE_DEFAULT));

    auto cutoffName = "Filter Cutoff";
    auto resName = "Filter Resonance";
    auto wetName = "Filter wet/dry";
    auto depthName = "Filter envelope depth";
    fRange cutoffRange(CUTOFF_MIN, CUTOFF_MAX);
    cutoffRange.setSkewForCentre(CUTOFF_CENTER);
    fRange resRange(RESONANCE_MIN, RESONANCE_MAX);
    resRange.setSkewForCentre(RESONANCE_CENTER);
    layout.add(std::make_unique<AudioParamFloat>(
        juce::ParameterID{ID::filterCutoff.toString(), 1}, cutoffName,
        cutoffRange, CUTOFF_DEFAULT));
    layout.add(std::make_unique<AudioParamFloat>(
        juce::ParameterID{ID::filterResonance.toString(), 1}, resName, resRange,
        RESONANCE_DEFAULT));
    layout.add(std::make_unique<AudioParamFloat>(
        juce::ParameterID{ID::filterWetDry.toString(), 1}, wetName, 0.0f, 1.0f,
        1.0f));
    layout.add(std::make_unique<AudioParamFloat>(
        juce::ParameterID{ID::filterEnvDepth.toString(), 1}, depthName, 0.0f,
        1.0f, 0.5f));
    juce::StringArray filterTypes;
    filterTypes.add("Low Pass");
    filterTypes.add("High Pass");
    filterTypes.add("Band Pass");
    layout.add(std::make_unique<AudioParamChoice>(
        juce::ParameterID{ID::filterType.toString(), 1}, "Filter Type",
        filterTypes, 0));

    fRange rateRange(RATE_MIN, RATE_MAX);
    for (int i = 0; i < NUM_LFOS; ++i) {
      auto iStr = juce::String(i);
      String rateId = ID::lfoRate + iStr;
      String depthId = ID::lfoDepth + iStr;
      String syncId = ID::lfoSync + iStr;
      String rateName = "LFO " + iStr + " rate";
      layout.add(std::make_unique<AudioParamFloat>(
          juce::ParameterID{rateId, 1}, rateName, rateRange, RATE_DEFAULT));
      layout.add(std::make_unique<AudioParamFloat>(
          juce::ParameterID{depthId, 1}, "LFO " + iStr + " depth", 0.0f, 1.0f,
          0.0f));
      layout.add(std::make_unique<AudioParamBool>(
          juce::ParameterID{syncId, 1}, "LFO " + iStr + " sync", false));
      juce::StringArray waves;
      waves.add("Sine");
      waves.add("Square");
      waves.add("Saw");
      waves.add("Tri");
      waves.add("Noise");
      String waveId = ID::lfoWave + iStr;
      auto waveName = "LFO " + iStr + " wave";
      layout.add(std::make_unique<AudioParamChoice>(
          juce::ParameterID{waveId, 1}, waveName, waves, 0));
      auto targets = LfoComponent::getTargetStrings();
      auto targetId = ID::lfoTarget + iStr;
      auto targetName = "LFO " + iStr + " target";
      layout.add(std::make_unique<AudioParamChoice>(
          juce::ParameterID{targetId, 1}, targetName, targets, 0));
    }

    for (int i = 0; i < NUM_OPERATORS; ++i) {
      juce::String iStr = juce::String(i);
      //! ocsillator parameters first
      // strings and parameters for the operator
      String ratioId = ID::operatorRatio + iStr;
      String ratioName = "Operator " + iStr + " ratio";
      String levelId = ID::operatorLevel + iStr;
      String levelName = "Operator " + iStr + " level";
      String indexId = ID::operatorModIndex + iStr;
      String indexName = "Operator " + iStr + " Mod Index";
      String outputId = ID::operatorAudible + iStr;
      String outputName = "Operator " + iStr + " audible";
      String panId = ID::operatorPan + iStr;
      String panName = "Operator " + iStr + " pan";
      fRange ratioRange(RATIO_MIN, RATIO_MAX);
      fRange modIndexRange(MODINDEX_MIN, MODINDEX_MAX);
      modIndexRange.setSkewForCentre(MODINDEX_CENTER);
      ratioRange.setSkewForCentre(RATIO_CENTER);
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{ratioId, 1}, ratioName, ratioRange, RATIO_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{levelId, 1}, levelName, 0.0f, 1.0f, 1.0f));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{indexId, 1}, indexName, modIndexRange,
          MODINDEX_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{panId, 1}, panName, PAN_MIN, PAN_MAX, PAN_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{outputId, 1}, outputName, false));
      //! on to envelope parameters
      String delayId = ID::envDelay + iStr;
      String delayName = "Operator " + iStr + " delay";
      String attackId = ID::envAttack + iStr;
      String attackName = "Operator " + iStr + " attack";
      String holdId = ID::envHold + iStr;
      String holdName = "Operator " + iStr + " hold";
      String decayId = ID::envDecay + iStr;
      String decayName = "Operator " + iStr + " decay";
      String sustainId = ID::envSustain + iStr;
      String sustainName = "Operator " + iStr + " sustain";
      String releaseId = ID::envRelease + iStr;
      String releaseName = "Operator " + iStr + " release";
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{delayId, 1}, delayName, delayRange, DELAY_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{attackId, 1}, attackName, attackRange,
          ATTACK_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{holdId, 1}, holdName, holdRange, HOLD_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{decayId, 1}, decayName, decayRange, DECAY_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{sustainId, 1}, sustainName, sustainRange,
          SUSTAIN_DEFAULT));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{releaseId, 1}, releaseName, releaseRange,
          RELEASE_DEFAULT));
      //! wave selection parameter
      juce::StringArray waves;
      waves.add("Sine");
      waves.add("Square");
      waves.add("Saw");
      waves.add("Tri");
      waves.add("Noise");
      auto waveId = ID::operatorWaveShape + iStr;
      auto waveName = "Operator " + iStr + " wave";
      layout.add(std::make_unique<juce::AudioParameterChoice>(
          juce::ParameterID{waveId, 1}, waveName, waves, 0));
      //! and onto the routing parameters
      for (int n = 0; n < NUM_OPERATORS; ++n) {
        juce::String nStr = juce::String(n);
        auto modId = iStr + "to" + nStr + "Param";
        auto modName = "Operator " + iStr + " to " + nStr;
        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{modId, 1}, modName, false));
      }
    }
    return layout;
  }
};
