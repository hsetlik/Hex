/*
  ==============================================================================

    LfoComponent.cpp
    Created: 25 Jun 2021 4:43:39pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LfoComponent.h"
#include "Identifiers.h"
#include "MathUtil.h"
#include "juce_core/juce_core.h"
DualModeSlider::DualModeSlider() : snapMode(false), bpm(120.0f) {
  noteLengths.push_back(std::make_pair(1, 32));
  noteLengths.push_back(std::make_pair(1, 24));
  noteLengths.push_back(std::make_pair(1, 18));
  noteLengths.push_back(std::make_pair(1, 16));
  noteLengths.push_back(std::make_pair(1, 12));
  noteLengths.push_back(std::make_pair(1, 8));
  noteLengths.push_back(std::make_pair(1, 6));
  noteLengths.push_back(std::make_pair(3, 16));
  noteLengths.push_back(std::make_pair(1, 4));
  noteLengths.push_back(std::make_pair(3, 8));
  noteLengths.push_back(std::make_pair(1, 3));
  noteLengths.push_back(std::make_pair(1, 2));
  noteLengths.push_back(std::make_pair(7, 16));
  noteLengths.push_back(std::make_pair(5, 8));
  noteLengths.push_back(std::make_pair(3, 4));
  noteLengths.push_back(std::make_pair(4, 4));
  calculateHzValues();
}
void DualModeSlider::calculateHzValues() {
  hzValues.clear();
  for (auto length : noteLengths) {
    auto value = NoteLength::frequencyHz(length.first, length.second, bpm);
    hzValues.push_back(value);
  }
}
double DualModeSlider::snapValue(double attemptedValue,
                                 juce::Slider::DragMode dragMode) {
  juce::ignoreUnused(dragMode);
  if (!snapMode)
    return attemptedValue;
  float minValue = std::numeric_limits<float>::max();
  size_t idx = 0;
  for (size_t i = 0; i < hzValues.size(); ++i) {
    if (std::abs(hzValues[i] - attemptedValue) < minValue) {
      minValue = std::fabs(hzValues[i] - (float)attemptedValue);
      idx = i;
    }
  }
  return (double)hzValues[idx];
}
std::pair<int, int> DualModeSlider::currentNoteLength() {
  auto attemptedValue = getValue();
  float minValue = std::numeric_limits<float>::max();
  size_t idx = 0;
  for (size_t i = 0; i < hzValues.size(); ++i) {
    if (std::abs(hzValues[i] - attemptedValue) < minValue) {
      minValue = std::fabs(hzValues[i] - (float)attemptedValue);
      idx = i;
    }
  }
  return noteLengths[idx];
}
bool DualModeSlider::setSync(int num, int denom, float tempo) {
  if (!snapMode)
    return false;
  setValue(NoteLength::frequencyHz(num, denom, tempo));
  return true;
}
//======================================================================================
DualModeLabel::DualModeLabel(DualModeSlider* s) : linkedSlider(s), bpm(120.0f) {
  linkedSlider->addListener(this);
  if (linkedSlider->inSnapMode()) {
    auto note = linkedSlider->currentNoteLength();
    lastStr = juce::String(note.first) + " / " + juce::String(note.second);
  } else {
    lastStr = juce::String((float)linkedSlider->getValue());
  }
  setText(lastStr, juce::dontSendNotification);
  setEditable(true);
}
void DualModeLabel::sliderValueChanged(juce::Slider* s) {
  jassert(s == linkedSlider);
  if (linkedSlider->inSnapMode()) {
    auto note = linkedSlider->currentNoteLength();
    setTextNoteLength(note.first, note.second);
  } else {
    setTextHz((float)linkedSlider->getValue());
  }
}
void DualModeLabel::setTextHz(float value) {
  setText(juce::String(value), juce::dontSendNotification);
}
void DualModeLabel::setTextNoteLength(int num, int denom) {
  auto str = juce::String(num) + " / " + juce::String(denom);
  setText(str, juce::dontSendNotification);
}
void DualModeLabel::textWasEdited() {
  auto str = getText();
  if (linkedSlider->inSnapMode() && str.contains("/")) {
    auto frac = stdu::stringAsFraction(str.toStdString());
    if (linkedSlider->setSync(frac.first, frac.second, bpm))
      setTextNoteLength(frac.first, frac.second);
    else
      setText(lastStr, juce::dontSendNotification);
  } else {
    float value = std::stof(str.toStdString());
    if (value >= linkedSlider->getMinimum() &&
        value <= linkedSlider->getMaximum()) {
      linkedSlider->setValue(value);
    }
  }
  lastStr = str;
}

void DualModeLabel::componentMovedOrResized(juce::Component& component,
                                            bool,
                                            bool) {
  auto& lf = getLookAndFeel();
  auto f = lf.getLabelFont(*this);
  auto borderSize = lf.getLabelBorderSize(*this);

  if (isAttachedOnLeft()) {
    auto width =
        std::min(juce::roundToInt(
                     juce::TextLayout::getStringWidth(f, getText()) * 3.5f) +
                     borderSize.getLeftAndRight(),
                 component.getX());

    setBounds(component.getX() - (int)width, component.getY(), (int)width,
              component.getHeight());
  } else {
    auto height = borderSize.getTopAndBottom() + 2 +
                  juce::roundToInt(f.getHeight() * 1.5f);

    auto stringWidth = juce::TextLayout::getStringWidth(f, getText()) * 3.5f;
    auto width = juce::roundToInt(stringWidth) + borderSize.getLeftAndRight();
    auto centerX =
        component.getX() + juce::roundToInt((float)component.getWidth() / 2.0f);
    auto x = centerX - juce::roundToInt((float)width / 2.0f);
    setBounds(x, component.getBottom() + 1, width, height);
  }
}

//======================================================================================

LfoComponent::LfoComponent(int i,
                           juce::AudioProcessor* proc,
                           GraphParamSet* gParams,
                           apvts* tree)
    : lfoIndex(i),
      linkedProcessor(proc),
      linkedParams(gParams),
      linkedTree(tree),
      rateLabel(&rateSlider),
      rateName("Rate"),
      depthLabel(&depthSlider),
      depthName("Depth"),
      waveSelect(i, tree, ID::lfoWave.toString()) {
  auto iStr = juce::String(lfoIndex);
  auto rateId = ID::lfoRate + iStr;
  rateAttach.reset(new sliderAttach(*linkedTree, rateId, rateSlider));
  auto syncId = ID::lfoSync + iStr;
  syncAttach.reset(new buttonAttach(*linkedTree, syncId, bpmToggle));
  auto targetId = ID::lfoTarget + iStr;
  targetAttach.reset(new comboBoxAttach(*linkedTree, targetId, targetBox));
  auto depthId = ID::lfoDepth + iStr;
  depthAttach.reset(new sliderAttach(*linkedTree, depthId, depthSlider));

  auto targets = getTargetStrings();
  targetBox.addItemList(targets, 1);

  addAndMakeVisible(&targetBox);

  addAndMakeVisible(&bpmToggle);
  bpmToggle.setButtonText("Sync");
  bpmToggle.setClickingTogglesState(true);
  bpmToggle.addListener(this);

  addAndMakeVisible(&rateSlider);
  rateSlider.setSliderStyle(juce::Slider::Rotary);
  rateSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  addAndMakeVisible(&rateLabel);

  addAndMakeVisible(&depthSlider);
  depthSlider.setSliderStyle(juce::Slider::Rotary);
  depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);

  depthLabel.attachToComponent(&depthSlider, false);
  rateLabel.attachToComponent(&rateSlider, false);

  addAndMakeVisible(&rateName);
  addAndMakeVisible(&depthName);

  rateName.attachToComponent(&rateSlider, false);
  depthName.attachToComponent(&depthSlider, false);

  addAndMakeVisible(&depthLabel);
  addAndMakeVisible(&waveSelect);

  startTimerHz(2);

  rateSlider.setLookAndFeel(&lnf);
  rateLabel.setLookAndFeel(&lnf);

  depthSlider.setLookAndFeel(&lnf);
  depthLabel.setLookAndFeel(&lnf);

  targetBox.setLookAndFeel(&lnf);

  targetBox.setSelectedId(1);

  prepare();
}

LfoComponent::~LfoComponent() {
  rateSlider.setLookAndFeel(nullptr);
  rateLabel.setLookAndFeel(nullptr);

  depthSlider.setLookAndFeel(nullptr);
  depthLabel.setLookAndFeel(nullptr);

  targetBox.setLookAndFeel(nullptr);
}

void LfoComponent::buttonClicked(juce::Button*) {
  rateSlider.toggleSnapMode();
  rateLabel.textWasEdited();
}

void LfoComponent::prepare() {
  // linkedProcessor->getPlayHead()->getCurrentPosition (currentPos);
  bpm = (float)currentPos.bpm;
  rateLabel.setBpm(bpm);
}

void LfoComponent::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float dX = fBounds.getWidth() / 16.0f;
  frect_t rateBox = {dX, dX, 5.0f * dX, 5.0f * dX};
  frect_t depthBounds = {dX, 7.0f * dX, 5.0f * dX, 5.0f * dX};
  frect_t bpmBounds = {7.0f * dX, dX, 3.0f * dX, 1.5f * dX};
  frect_t targetBounds = {8.0f * dX, 4.0f * dX, 8.0f * dX, 2.0f * dX};
  frect_t waveBounds = {6.0f * dX, 7.0f * dX, 10.0f * dX, 3.0f * dX};
  rateSlider.setBounds(rateBox.reduced(3.0f).toNearestInt());
  depthSlider.setBounds(depthBounds.reduced(3.0f).toNearestInt());
  bpmToggle.setBounds(bpmBounds.toNearestInt());
  targetBox.setBounds(targetBounds.toNearestInt());
  waveSelect.setBounds(waveBounds.toNearestInt());
  // auto dX = getWidth() / 16;
  // rateSlider.setBounds (dX, dX, 5 * dX, 5 * dX);
  // ComponentUtil::cushionByFraction (rateSlider, 6, 6);
  // depthSlider.setBounds (dX, 7 * dX, 5 * dX, 5 * dX);
  // ComponentUtil::cushionByFraction (depthSlider, 6, 6);
  // bpmToggle.setBounds (7 * dX, dX, 3 * dX, 1.5f * dX);
  // targetBox.setBounds (8 * dX, 4 * dX, 8 * dX, 2 * dX);
  // waveSelect.setBounds (6 * dX, 7 * dX, 10 * dX, 3 * dX);
}

void LfoComponent::timerCallback() {
  prepare();
}

juce::StringArray LfoComponent::getTargetStrings() {
  juce::StringArray vec;
  vec.add("No Target");
  for (int i = 0; i < NUM_OPERATORS; ++i) {
    auto iStr = juce::String(i + 1);
    vec.add("Operator " + iStr + " level");
  }
  vec.add("Filter Cutoff");
  return vec;
}
