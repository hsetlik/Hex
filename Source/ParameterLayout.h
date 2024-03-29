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
using fRange = juce::NormalisableRange<float>;
class HexParameters
{
public:
    static apvts::ParameterLayout createLayout()
    {
       
        apvts::ParameterLayout layout;
        auto delayId = "filterDelayParam";
        auto delayName = "Filter Delay";
        auto attackId = "filterAttackParam";
        auto attackName = "Filter Attack";
        auto holdId = "filterHoldParam";
        auto holdName = "Filter Hold";
        auto decayId = "filterDecayParam";
        auto decayName = "Filter Decay";
        auto sustainId = "filterSustainParam";
        auto sustainName = "Filter Sustain";
        auto releaseId = "filterReleaseParam";
        auto releaseName = "Filter Release";
        fRange delayRange (DELAY_MIN, DELAY_MAX);
        delayRange.setSkewForCentre (DELAY_CENTER);
        fRange attackRange (ATTACK_MIN, ATTACK_MAX);
        attackRange.setSkewForCentre (ATTACK_CENTER);
        fRange holdRange (HOLD_MIN, HOLD_MAX);
        holdRange.setSkewForCentre (HOLD_CENTER);
        fRange decayRange (DECAY_MIN, DECAY_MAX);
        decayRange.setSkewForCentre (DECAY_CENTER);
        fRange sustainRange (SUSTAIN_MIN, SUSTAIN_MAX);
        fRange releaseRange (RELEASE_MIN, RELEASE_MAX);
        releaseRange.setSkewForCentre (RELEASE_CENTER);
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {delayId, 1}, delayName, delayRange, DELAY_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {attackId, 1}, attackName, attackRange, ATTACK_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {holdId, 1}, holdName, holdRange, HOLD_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {decayId, 1}, decayName, decayRange, DECAY_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {sustainId, 1}, sustainName, sustainRange, SUSTAIN_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {releaseId, 1}, releaseName, releaseRange, RELEASE_DEFAULT));
        
        auto cutoffId = "cutoffParam";
        auto cutoffName = "Filter Cutoff";
        auto resId = "resonanceParam";
        auto resName = "Filter Resonance";
        auto wetId = "wetDryParam";
        auto wetName = "Filter wet/dry";
        auto depthId = "depthParam";
        auto depthName = "Filter envelope depth";
        fRange cutoffRange (CUTOFF_MIN, CUTOFF_MAX);
        cutoffRange.setSkewForCentre (CUTOFF_CENTER);
        fRange resRange (RESONANCE_MIN, RESONANCE_MAX);
        resRange.setSkewForCentre (RESONANCE_CENTER);
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {cutoffId, 1}, cutoffName, cutoffRange, CUTOFF_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {resId, 1}, resName, resRange, RESONANCE_DEFAULT));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {wetId, 1}, wetName, 0.0f, 1.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {depthId, 1}, depthName, 0.0f, 1.0f, 0.5f));
        juce::StringArray filterTypes;
        filterTypes.add ("Low Pass");
        filterTypes.add ("High Pass");
        filterTypes.add ("Band Pass");
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {"filterTypeParam", 1}, "Filter Type", filterTypes, 0));
        
        fRange rateRange (RATE_MIN, RATE_MAX);
        for (int i = 0; i < NUM_LFOS; ++i)
        {
            auto iStr = juce::String (i);
            auto rateId = "lfoRateParam" + iStr;
            auto depthId = "lfoDepthParam" + iStr;
            auto syncId = "lfoSyncParam" + iStr;
            auto rateName = "LFO " + iStr + " rate";
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {rateId, 1}, rateName, rateRange, RATE_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {depthId, 1}, "LFO " + iStr + " depth", 0.0f, 1.0f, 0.0f));
            layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {syncId, 1}, "LFO " + iStr + " sync", false));
            juce::StringArray waves;
            waves.add ("Sine");
            waves.add ("Square");
            waves.add ("Saw");
            waves.add ("Tri");
            waves.add ("Noise");
            auto waveId = "lfoWaveParam" + iStr;
            auto waveName = "LFO " + iStr + " wave";
            layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {waveId, 1}, waveName, waves, 0));
            auto targets = LfoComponent::getTargetStrings();
            auto targetId = "lfoTargetParam" + iStr;
            auto targetName = "LFO " + iStr + " target";
            layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {targetId, 1}, targetName, targets, 0));
        }
        
        
        for (int i = 0; i < NUM_OPERATORS; ++i)
        {
            juce::String iStr = juce::String (i);
            //! ocsillator parameters first
            //strings and parameters for the operator
            auto ratioId = "ratioParam" + iStr;
            auto ratioName = "Operator " + iStr + " ratio";
            auto levelId = "levelParam" + iStr;
            auto levelName = "Operator " + iStr + " level";
            auto indexId = "indexParam" + iStr;
            auto indexName = "Operator " + iStr + " Mod Index";
            auto outputId = "audibleParam" + iStr;
            auto outputName = "Operator " + iStr + " audible";
            auto panId = "panParam" + iStr;
            auto panName = "Operator " + iStr + " pan";
            fRange ratioRange (RATIO_MIN, RATIO_MAX);
            fRange modIndexRange (MODINDEX_MIN, MODINDEX_MAX);
            modIndexRange.setSkewForCentre (MODINDEX_CENTER);
            ratioRange.setSkewForCentre (RATIO_CENTER);
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {ratioId, 1}, ratioName, ratioRange, RATIO_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {levelId, 1}, levelName, 0.0f, 1.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {indexId, 1}, indexName, modIndexRange, MODINDEX_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {panId, 1}, panName, PAN_MIN, PAN_MAX, PAN_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {outputId, 1}, outputName, false));
            //! on to envelope parameters
            auto delayId = "delayParam" + iStr;
            auto delayName = "Operator " + iStr + " delay";
            auto attackId = "attackParam" + iStr;
            auto attackName = "Operator " + iStr + " attack";
            auto holdId = "holdParam" + iStr;
            auto holdName = "Operator " + iStr + " hold";
            auto decayId = "decayParam" + iStr;
            auto decayName = "Operator " + iStr + " decay";
            auto sustainId = "sustainParam" + iStr;
            auto sustainName = "Operator " + iStr + " sustain";
            auto releaseId = "releaseParam" + iStr;
            auto releaseName = "Operator " + iStr + " release";
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {delayId, 1}, delayName, delayRange, DELAY_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {attackId, 1}, attackName, attackRange, ATTACK_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {holdId, 1}, holdName, holdRange, HOLD_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {decayId, 1}, decayName, decayRange, DECAY_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {sustainId, 1}, sustainName, sustainRange, SUSTAIN_DEFAULT));
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {releaseId, 1}, releaseName, releaseRange, RELEASE_DEFAULT));
            //! wave selection parameter
            juce::StringArray waves;
            waves.add ("Sine");
            waves.add ("Square");
            waves.add ("Saw");
            waves.add ("Tri");
            waves.add ("Noise");
            auto waveId = "waveParam" + iStr;
            auto waveName = "Operator " + iStr + " wave";
            layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {waveId, 1}, waveName, waves, 0));
            //! and onto the routing parameters
            for (int n = 0; n < NUM_OPERATORS; ++n)
            {
                juce::String nStr = juce::String (n);
                auto modId = iStr + "to" + nStr + "Param";
                auto modName = "Operator " + iStr + " to " + nStr;
                layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {modId, 1}, modName, false));
            }
        }
        return layout;
    }
};

