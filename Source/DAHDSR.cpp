/*
  ==============================================================================

    DAHDSR.cpp
    Created: 6 Oct 2020 1:07:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "DAHDSR.h"
void DAHDSR::enterPhase(envPhase newPhase)
{
    currentPhase = newPhase;
    samplesIntoPhase = 0;
    switch(newPhase)
    {
        case delayPhase:
        {
            _startLevel = minLevel;
            _endLevel = minLevel;
            samplesInPhase = delayTime * (sampleRate / 1000);
            factor = factorFor(_startLevel, _endLevel, delayTime);
            break;
        }
        case attackPhase:
        {
            _startLevel = minLevel;
            _endLevel = 1.0f;
            samplesInPhase = attackTime * (sampleRate / 1000);
            factor = factorFor(_startLevel, _endLevel, attackTime);
            break;
        }
        case holdPhase:
        {
            _startLevel = 1.0f;
            _endLevel = 1.0f;
            samplesInPhase = holdTime * (sampleRate / 1000);
            factor = factorFor(_startLevel, _endLevel, holdTime);
            break;
        }
        case decayPhase:
        {
            _startLevel = 1.0f;
            _endLevel = sustainLevel;
            samplesInPhase = decayTime * sampleRate / 1000;
            factor = factorFor(_startLevel, _endLevel, decayTime);
            break;
        }
        case sustainPhase:
        {
            _startLevel = sustainLevel;
            _endLevel = sustainLevel;
            samplesInPhase = 1000000;
            factor = 1.0f;
            break;
        }
        case releasePhase:
        {
            _startLevel = sustainLevel;
            _endLevel = minLevel;
            samplesInPhase = releaseTime * sampleRate / 1000;
            factor = factorFor(_startLevel, _endLevel, releaseTime);
            break;
        }
        case noteOff:
        {
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
float DAHDSR::process(float input)
{
    lastOutput = output;
    updatePhase();
    ++samplesIntoPhase;
    output *= factor;
    if(std::abs(lastOutput - output) > 0.2f)
    {
        printf("Envelope click\n");
    }
    return input * output;
}

void DAHDSR::killQuick(float msFade)
{
    //! function to bring down the envelope output very quickly (but not immediately) for voice stealing
    currentPhase = releasePhase;
    _startLevel = output;
    _endLevel = minLevel;
    samplesInPhase = msFade * sampleRate / 1000;
    factor = factorFor(_startLevel, _endLevel, msFade);
}

void DAHDSR::printDebug()
{
    printf("This output: %f\n", output);
    printf("Last output: %f\n", lastOutput);
}
