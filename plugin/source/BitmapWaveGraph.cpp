#include "GUI/BitmapWaveGraph.h"
#include "GUI/Color.h"
#include "RingBuffer.h"
#include "juce_audio_basics/juce_audio_basics.h"

int BitmapWaveGraph::firstRisingEdge() const {
  auto* data = readBuffer.getReadPointer(0);
  for (int i = 1; i < readBuffer.getNumSamples(); ++i) {
    auto& prev = data[i - 1];
    auto& current = data[i];
    if (prev < 0.0f && current > 0.0f)
      return i - 1;
  }
  return -1;
}

static float s_levelToYPos(float level) {
  static const float y0 = (float)GRAPH_PX_HEIGHT / 2.0f;
  static const float amplitude = y0;
  return y0 + (level * amplitude);
}

void BitmapWaveGraph::computeWavePoints(int zCrossingIdx) {
  static const float y0 = (float)GRAPH_PX_HEIGHT / 2.0f;
  auto* data = readBuffer.getReadPointer(0);
  const double samplesPerCycle = sampleRate / fundamental;
  const double dSample = (samplesPerCycle * 2.0) / (double)WAVE_GRAPH_PTS;
  double fSample = (double)zCrossingIdx;
  if (zCrossingIdx != -1) {
    for (int i = 0; i < WAVE_GRAPH_PTS; ++i) {
      int readIdx = (int)fSample % readBuffer.getNumSamples();
      wavePoints[(size_t)i] = s_levelToYPos(data[readIdx]);
      fSample += dSample;
    }
  } else {
    std::fill(wavePoints.begin(), wavePoints.end(), y0);
  }
}
//===========================================================================

BitmapWaveGraph::BitmapWaveGraph(GraphParamSet* gp, RingBuffer<float>* rBuf)
    : ringBuf(rBuf),
      params(gp),
      readBuffer(2, RING_BUFFER_READ_SIZE * 5),
      imgA(juce::Image::RGB, GRAPH_PX_WIDTH, GRAPH_PX_HEIGHT, true),
      imgB(juce::Image::RGB, GRAPH_PX_WIDTH, GRAPH_PX_HEIGHT, true) {
  startTimerHz(WAVE_GRAPH_HZ);
  // fill the read buffers with zero to to avoid
  // startup weirdness
  readBuffer.clear();
}

void BitmapWaveGraph::timerCallback() {
  triggerAsyncUpdate();
}

void BitmapWaveGraph::handleAsyncUpdate() {
  // 1. read in the buffer and find the fundamental
  ringBuf->readSamples(readBuffer, RING_BUFFER_READ_SIZE * 5);
  const int idx = params->lastTriggeredVoice.load();
  fundamental = params->voiceFundamentals[idx].load();
  jassert(!std::isnan(fundamental) && fundamental < 20000.0);
  // 2. find the index of the first zero-crossing and compute
  // the y-values
  auto zCrossing = firstRisingEdge();
  computeWavePoints(zCrossing);
  // 3. redraw the idle image
  juce::Graphics g(*idleImg);
  auto fBounds = idleImg->getBounds().toFloat();
  g.setColour(UXPalette::darkBkgnd);
  g.fillRect(fBounds);

  const float dX = fBounds.getWidth() / (float)WAVE_GRAPH_PTS;
  juce::Path p;
  float xPos = 0.0f;
  p.startNewSubPath(xPos, wavePoints[0]);
  for (int i = 1; i < WAVE_GRAPH_PTS; ++i) {
    xPos += dX;
    p.lineTo(xPos, wavePoints[(size_t)i]);
  }
  juce::PathStrokeType pst(2.0f);
  g.setColour(UXPalette::highlight);
  g.strokePath(p, pst);
  // 4. Switch the image pointers
  juce::ScopedLock sl(critSection);
  auto* prevActive = activeImg;
  activeImg = idleImg;
  idleImg = prevActive;
  repaint();
}

void BitmapWaveGraph::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.drawImage(*activeImg, fBounds);
}
