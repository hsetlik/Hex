/*
  ==============================================================================

    Filter.cpp
    Created: 14 Jun 2021 10:59:58am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Filter.h"
StereoFilter::StereoFilter (int voiceIdx) :
cutoffVal (2500.0f),
resonanceVal (1.0f),
rateVal (44100.0f),
envDepth (0.5f),
wetLevel (1.0f),
voiceIndex (voiceIdx),
currentType (LoPass),
lFilter (std::make_unique<LibLowPass>()),
rFilter (std::make_unique<LibLowPass>())
{
    
}
void StereoFilter::setType (int filterType)
{
    auto nType = (FilterType)filterType;
    if (currentType != nType)
    {
        currentType = nType;
        triggerAsyncUpdate();
    }
}

void StereoFilter::handleAsyncUpdate()
{
    switch (currentType)
    {
        case LoPass:
        {
            lFilter.reset (new LibLowPass());
            rFilter.reset (new LibLowPass());
            break;
        }
        case HiPass:
        {
            lFilter.reset (new LibHiPass());
            rFilter.reset (new LibHiPass());
            break;
        }
        case BandPass:
        {
            lFilter.reset (new LibBandPass());
            rFilter.reset (new LibBandPass());
            break;
        }
    }
    //! make sure these parameters stay the same when the unique_ptr gets reset
    setSampleRate (rateVal);
    setCutoff (cutoffVal);
    setResonance (resonanceVal);
}

void StereoFilter::filterBlock (juce::AudioBuffer<float> &buffer)
{
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        buffer.setSample (0, i, processLeft (buffer.getSample (0, i)));
        buffer.setSample (1, i, processRight (buffer.getSample (1, i)));
    }
}
