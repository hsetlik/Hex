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

class HexParameters
{
public:
    static apvts::ParameterLayout createLayout()
    {
        using fRange = juce::NormalisableRange<float>;
        apvts::ParameterLayout layout;
        for(int i = 0; i < NUM_OPERATORS; ++i)
        {
            juce::String iStr = juce::String(i);
            //! ocsillator parameters first
            //strings and parameters for the operator
            auto ratioId = "ratioParam" + iStr;
            auto ratioName = "Operator " + iStr + " ratio";
            //auto levelId = "levelParam" + iStr;
            //auto levelName = "Operator " + iStr + " level";
            auto indexId = "indexParam" + iStr;
            auto indexName = "Operator " + iStr + " Mod Index";
            auto outputId = "audibleParam" + iStr;
            auto outputName = "Operator " + iStr + " audible";
            auto panId = "panParam" + iStr;
            auto panName = "Operator " + iStr + " pan";
            fRange ratioRange(RATIO_MIN, RATIO_MAX);
            fRange modIndexRange(MODINDEX_MIN, MODINDEX_MAX);
            modIndexRange.setSkewForCentre(MODINDEX_CENTER);
            ratioRange.setSkewForCentre(RATIO_CENTER);
            layout.add(std::make_unique<juce::AudioParameterFloat>(ratioId, ratioName, ratioRange, RATIO_DEFAULT));
            //layout.add(std::make_unique<juce::AudioParameterFloat>(levelId, levelName, 0.0f, 1.0f, 1.0f));
            layout.add(std::make_unique<juce::AudioParameterFloat>(indexId, indexName, modIndexRange, MODINDEX_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterFloat>(panId, panName, PAN_MIN, PAN_MAX, PAN_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterBool>(outputId, outputName, false));
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
            layout.add(std::make_unique<juce::AudioParameterFloat>(delayId, delayName, delayRange, DELAY_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterFloat>(attackId, attackName, attackRange, ATTACK_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterFloat>(holdId, holdName, holdRange, HOLD_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterFloat>(decayId, decayName, decayRange, DECAY_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterFloat>(sustainId, sustainName, sustainRange, SUSTAIN_DEFAULT));
            layout.add(std::make_unique<juce::AudioParameterFloat>(releaseId, releaseName, releaseRange, RELEASE_DEFAULT));
            //! and onto the routing parameters
            for(int n = 0; n < NUM_OPERATORS; ++n)
            {
                juce::String nStr = juce::String(n);
                auto modId = iStr + "to" + nStr + "Param";
                auto modName = "Operator " + iStr + " to " + nStr;
                layout.add(std::make_unique<juce::AudioParameterBool>(modId, modName, false));
            }
        }
        return layout;
    }
};

