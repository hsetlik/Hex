#pragma once

#include "HexHeader.h"
#include "Assets.h"

class WaveformButton : public juce::Button {
private:
  const int shapeID;

public:
  WaveformButton(int waveShape);
  ~WaveformButton() override {}
  void paintButton(juce::Graphics& g, bool down, bool highlighted) override;
  int waveID() const { return shapeID; }
};

//===================================================

class WaveformSelector : public Component, public juce::Button::Listener {
private:
  juce::OwnedArray<WaveformButton> buttons;
  const String paramName;
  param_attach_ptr attach;
  void paramChangeCallback(float val);

public:
  WaveformSelector(apvts& tree, const String& paramName);
  ~WaveformSelector() override;
  void buttonClicked(juce::Button* b) override;
  void resized() override;
};
