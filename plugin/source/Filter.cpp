/*
  ==============================================================================

    Filter.cpp
    Created: 14 Jun 2021 10:59:58am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Filter.h"
#include "DAHDSR.h"
StereoFilter::StereoFilter(EnvelopeLUTGroup* luts, int voiceIdx)
    : env(&luts->filterEnv),
      cutoffVal(2500.0f),
      resonanceVal(1.0f),
      rateVal(44100.0f),
      envDepth(0.5f),
      wetLevel(1.0f),
      voiceIndex(voiceIdx),
      currentType(LoPass),
      lFilter(std::make_unique<LibLowPass>()),
      rFilter(std::make_unique<LibLowPass>()) {}
void StereoFilter::setType(int filterType) {
  auto nType = (FilterType)filterType;
  if (currentType != nType) {
    currentType = nType;
    triggerAsyncUpdate();
  }
}

void StereoFilter::handleAsyncUpdate() {
  switch (currentType) {
    case LoPass: {
      lFilter.reset(new LibLowPass());
      rFilter.reset(new LibLowPass());
      break;
    }
    case HiPass: {
      lFilter.reset(new LibHiPass());
      rFilter.reset(new LibHiPass());
      break;
    }
    case BandPass: {
      lFilter.reset(new LibBandPass());
      rFilter.reset(new LibBandPass());
      break;
    }
  }
  //! make sure these parameters stay the same when the unique_ptr gets reset
  setSampleRate(rateVal);
  setCutoff(cutoffVal);
  setResonance(resonanceVal);
}
