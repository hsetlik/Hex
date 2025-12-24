#include "GUI/WaveformSelector.h"
#include "GUI/Assets.h"
#include "Identifiers.h"
#include "juce_events/juce_events.h"

WaveformButton::WaveformButton(int waveShape)
    : juce::Button("Waveform" + String(waveShape)), shapeID(waveShape) {
  // setClickingTogglesState(true);
}

void WaveformButton::paintButton(juce::Graphics& g,
                                 bool down,
                                 bool highlighted) {
  juce::ignoreUnused(down, highlighted);
  auto fBounds = getLocalBounds().toFloat();
  auto& img = Assets::getWaveImage((Assets::WaveID)shapeID, getToggleState());
  g.drawImage(img, fBounds);
}

//===================================================

WaveformSelector::WaveformSelector(apvts& tree, const String& name)
    : paramName(name) {
  // set up the buttons
  for (int i = 0; i < 5; ++i) {
    auto* newBtn = buttons.add(new WaveformButton(i));
    addAndMakeVisible(newBtn);
    newBtn->addListener(this);
  }
  // set up the param attachment
  auto* param = tree.getParameter(paramName);
  jassert(param != nullptr);
  auto callback = [this](float value) { paramChangeCallback(value); };
  attach.reset(new ParamAttachment(*param, callback, nullptr));
  attach->sendInitialUpdate();
}

WaveformSelector::~WaveformSelector() {
  for (int i = 0; i < 5; ++i) {
    buttons[i]->removeListener(this);
  }
}

void WaveformSelector::paramChangeCallback(float val) {
  const int activeIdx = (int)val;
  for (int i = 0; i < 5; ++i) {
    buttons[i]->setToggleState(activeIdx == i, juce::dontSendNotification);
  }
  buttons[activeIdx]->toFront(false);
}

void WaveformSelector::buttonClicked(juce::Button* b) {
  auto* waveBtn = dynamic_cast<WaveformButton*>(b);
  jassert(waveBtn != nullptr);
  const float fVal = (float)waveBtn->waveID();
  attach->setValueAsCompleteGesture(fVal);
}

void WaveformSelector::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float btnWidth = fBounds.getWidth() / 5.0f;
  const float btnHeight = fBounds.getHeight();
  float xPos = 0.0f;
  for (int i = 0; i < 5; ++i) {
    frect_t btnBounds = {xPos, 0.0f, btnWidth, btnHeight};
    buttons[i]->setBounds(btnBounds.toNearestInt());
    xPos += btnWidth;
  }
}
