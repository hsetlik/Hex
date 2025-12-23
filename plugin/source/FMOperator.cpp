/*
  ==============================================================================

    FMOperator.cpp
    Created: 4 Jun 2021 2:15:51pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Audio/FMOperator.h"
#include "Audio/DAHDSR.h"
#include "MathUtil.h"

FMOperator::FMOperator(int opIndex, EnvelopeLUTGroup* luts)
    : audible(false),
      index(opIndex),
      vEnv(&luts->operatorEnv[opIndex]),
      modIndex(0.0f),
      modOffset(0.0f),
      pan(0.5f),
      level(1.0f),
      lastOutMono(0.0f),
      lastOutL(0.0f),
      lastOutR(0.0f) {}

void FMOperator::setSampleRate(double rate) {
  oscillator.setSampleRate(rate);
}

void FMOperator::tick(double fundamental) {
  lastOutMono = vEnv.process(
      oscillator.getSample((fundamental * baseRatio) + (modIndex * modOffset)) *
      level);
  lastOutL = lastOutMono * pan;
  lastOutR = lastOutMono * (1.0f - pan);
}

void FMOperator::tick(double fundamental, float modValue) {
  lastOutMono = vEnv.process(
      oscillator.getSample((fundamental * baseRatio) + (modIndex * modOffset)) *
      MathUtil::fLerp(level, 1.0f, modValue));
  lastOutL = lastOutMono * pan;
  lastOutR = lastOutMono * (1.0f - pan);
}
