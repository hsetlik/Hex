/*
  ==============================================================================

    Filter.cpp
    Created: 14 Jun 2021 10:59:58am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Filter.h"
HexFilter::HexFilter(int voiceIdx) :
cutoffVal(2500.0f),
resonanceVal(1.0f),
rateVal(44100.0f),
voiceIndex(voiceIdx),
currentType(LoPass),
pFilter(std::make_unique<LibLowPass>())
{
    
}

void HexFilter::setType(int filterType)
{
    auto nType = (FilterType)filterType;
    if(currentType != nType)
    {
        currentType = nType;
        triggerAsyncUpdate();
    }
}

void HexFilter::handleAsyncUpdate()
{
    switch(currentType)
    {
        case LoPass:
        {
            pFilter.reset(new LibLowPass());
            break;
        }
        case HiPass:
        {
            pFilter.reset(new LibHiPass());
            break;
        }
        case BandPass:
        {
            pFilter.reset(new LibBandPass());
            break;
        }
    }
    //! make sure these parameters stay the same when the unique_ptr gets reset
    setSampleRate(rateVal);
    setCutoff(cutoffVal);
    setResonance(resonanceVal);
}
