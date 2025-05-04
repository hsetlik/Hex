#pragma once

#include "RingBuffer.h"
#include "Synthesizer.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"

#define GRAPH_PX_WIDTH 512
#define GRAPH_PX_HEIGHT 512
#define WAVE_GRAPH_HZ 24
#define WAVE_GRAPH_PTS 256

class BitmapWaveGraph : public juce::Component,
                        public juce::Timer,
                        private juce::AsyncUpdater {
private:
  RingBuffer<float>* const ringBuf;
  GraphParamSet* const params;
  juce::AudioBuffer<float> readBuffer;
  std::array<float, WAVE_GRAPH_PTS> wavePoints;
  double fundamental = 0.0f;
  double sampleRate = 44100.0;

  juce::Image imgA;
  juce::Image imgB;
  juce::Image* activeImg = &imgA;
  juce::Image* idleImg = &imgB;

  juce::CriticalSection critSection;
  int firstRisingEdge() const;
  void computeWavePoints(int zCrossing);

  void handleAsyncUpdate() override;

public:
  BitmapWaveGraph(GraphParamSet* gp, RingBuffer<float>* rBuf);
  void timerCallback() override;
  void paint(juce::Graphics& g) override;
};
