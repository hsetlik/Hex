//===================================================
#include "GUI/EnvelopeComponent.h"

EnvelopeComponent::DAHDSRGraph::DAHDSRGraph(EnvelopeComponent* env)
    : pDelay(&env->delaySlider),
      pAttack(&env->attackSlider),
      pHold(&env->holdSlider),
      pDecay(&env->decaySlider),
      pSustain(&env->sustainSlider),
      pRelease(&env->releaseSlider),
      delayVal(DELAY_DEFAULT),
      attackVal(ATTACK_DEFAULT),
      holdVal(HOLD_DEFAULT),
      decayVal(DECAY_DEFAULT),
      sustainVal(SUSTAIN_DEFAULT),
      releaseVal(RELEASE_DEFAULT),
      needsRepaint(false) {
  startTimerHz(REPAINT_FPS);
}
void EnvelopeComponent::DAHDSRGraph::timerCallback() {
  if (needsRepaint) {
    repaint();
    needsRepaint = false;
  }
}
void EnvelopeComponent::DAHDSRGraph::sliderValueChanged(juce::Slider* slider) {
  if (slider == pDelay) {
    delayVal = (float)slider->getValue();
    slider->setTooltip(juce::String(delayVal));
    needsRepaint = true;
  } else if (slider == pAttack) {
    attackVal = (float)slider->getValue();
    slider->setTooltip(juce::String(attackVal));
    needsRepaint = true;
  } else if (slider == pHold) {
    holdVal = (float)slider->getValue();
    slider->setTooltip(juce::String(holdVal));
    needsRepaint = true;
  } else if (slider == pDecay) {
    decayVal = (float)slider->getValue();
    slider->setTooltip(juce::String(decayVal));
    needsRepaint = true;
  } else if (slider == pSustain) {
    sustainVal = (float)slider->getValue();
    slider->setTooltip(juce::String(sustainVal));
    needsRepaint = true;
  } else if (slider == pRelease) {
    releaseVal = (float)slider->getValue();
    slider->setTooltip(juce::String(releaseVal));
    needsRepaint = true;
  }
}
void EnvelopeComponent::DAHDSRGraph::paint(juce::Graphics& g) {
  g.fillAll(UXPalette::darkGray);
  auto area = getLocalBounds().toFloat();
  auto timeTotal = delayVal + attackVal + holdVal + decayVal + releaseVal;
  juce::Path trace;
  trace.startNewSubPath(0.0f, area.getHeight());
  trace.lineTo(delayVal, area.getHeight());
  trace.lineTo(delayVal + attackVal, 0.0f);
  trace.lineTo(delayVal + attackVal + holdVal, 0.0f);
  auto sustainY = (1.0f - sustainVal) * area.getHeight();

  float sustainLength = timeTotal * 0.25f;
  trace.lineTo(delayVal + attackVal + holdVal + decayVal, sustainY);
  trace.lineTo(delayVal + attackVal + holdVal + decayVal + sustainLength,
               sustainY);
  trace.lineTo(timeTotal + sustainLength, area.getHeight());
  trace.scaleToFit(0.0f, 5.0f, area.getWidth(), (area.getHeight() - 5.0f),
                   false);

  auto stroke = juce::PathStrokeType(1.0f);
  g.setColour(UXPalette::highlight);
  g.strokePath(trace, stroke);
}
//==============================================================================
EnvelopeComponent::LevelMeter::LevelMeter(int idx,
                                          GraphParamSet* params,
                                          bool filter)
    : envIndex(idx),
      isFilter(filter),
      linkedParams(params),
      level(0.0f),
      lastVoice(0) {
  startTimerHz(REPAINT_FPS);
}

void EnvelopeComponent::LevelMeter::timerCallback() {
  triggerAsyncUpdate();
}

void EnvelopeComponent::LevelMeter::handleAsyncUpdate() {
  lastVoice = linkedParams->lastTriggeredVoice.load();
  float newLevel = 0.0f;
  if (linkedParams->filterLevels[lastVoice].is_lock_free()) {
    if (!isFilter)
      newLevel = linkedParams->levels[lastVoice][envIndex].load();
    else
      newLevel = linkedParams->filterLevels[lastVoice].load();
  } else
    printf("Warning: Graph parameters have locks!\n");
  if (!fequal(level, newLevel)) {
    level = newLevel;
    repaint();
  }
}

void EnvelopeComponent::LevelMeter::paint(juce::Graphics& g) {
  auto bBounds = getLocalBounds().toFloat();
  auto lBounds =
      juce::Rectangle<float>(bBounds.getX(), bBounds.getY(), bBounds.getWidth(),
                             bBounds.getHeight() * (1.0f - level));
  g.setColour(UXPalette::highlight);
  g.fillRect(bBounds);
  g.setColour(UXPalette::darkBlue);
  g.fillRect(lBounds);
}
//==============================================================================
EnvelopeComponent::EnvelopeComponent(int idx,
                                     apvts* tree,
                                     GraphParamSet* gParams,
                                     bool isFilterComp)
    : opIndex(idx),
      isFilter(isFilterComp),
      linkedTree(tree),
      graph(this),
      meter(idx, gParams, isFilter),
      delayName("Delay"),
      attackName("Attack"),
      holdName("Hold"),
      decayName("Decay"),
      sustainName("Sustain"),
      releaseName("Release") {
  SliderUtil::setVerticalLinearNoBox(delaySlider);
  SliderUtil::setVerticalLinearNoBox(attackSlider);
  SliderUtil::setVerticalLinearNoBox(holdSlider);
  SliderUtil::setVerticalLinearNoBox(decaySlider);
  SliderUtil::setVerticalLinearNoBox(sustainSlider);
  SliderUtil::setVerticalLinearNoBox(releaseSlider);

  addAndMakeVisible(&delaySlider);
  addAndMakeVisible(&attackSlider);
  addAndMakeVisible(&holdSlider);
  addAndMakeVisible(&decaySlider);
  addAndMakeVisible(&sustainSlider);
  addAndMakeVisible(&releaseSlider);

  addAndMakeVisible(&delayName);
  addAndMakeVisible(&attackName);
  addAndMakeVisible(&holdName);
  addAndMakeVisible(&decayName);
  addAndMakeVisible(&sustainName);
  addAndMakeVisible(&releaseName);

  delayName.attachToComponent(&delaySlider, false);
  attackName.attachToComponent(&attackSlider, false);
  holdName.attachToComponent(&holdSlider, false);
  decayName.attachToComponent(&decaySlider, false);
  sustainName.attachToComponent(&sustainSlider, false);
  releaseName.attachToComponent(&releaseSlider, false);

  delaySlider.addListener(&graph);
  attackSlider.addListener(&graph);
  holdSlider.addListener(&graph);
  decaySlider.addListener(&graph);
  sustainSlider.addListener(&graph);
  releaseSlider.addListener(&graph);

  // delaySlider.setLookAndFeel(&lnf);
  // attackSlider.setLookAndFeel(&lnf);
  // holdSlider.setLookAndFeel(&lnf);
  // decaySlider.setLookAndFeel(&lnf);
  // sustainSlider.setLookAndFeel(&lnf);
  // releaseSlider.setLookAndFeel(&lnf);

  addAndMakeVisible(&graph);

  addAndMakeVisible(&meter);

  auto iStr = juce::String(opIndex);
  String delayId = ID::envDelay + iStr;
  String attackId = ID::envAttack + iStr;
  String holdId = ID::envHold + iStr;
  String decayId = ID::envDecay + iStr;
  String sustainId = ID::envSustain + iStr;
  String releaseId = ID::envRelease + iStr;
  if (isFilter) {
    delayId = ID::filterEnvDelay.toString();
    attackId = ID::filterEnvAttack.toString();
    holdId = ID::filterEnvHold.toString();
    decayId = ID::filterEnvDecay.toString();
    sustainId = ID::filterEnvSustain.toString();
    releaseId = ID::filterEnvRelease.toString();
  }

  delayAttach.reset(new sliderAttach(*linkedTree, delayId, delaySlider));
  attackAttach.reset(new sliderAttach(*linkedTree, attackId, attackSlider));
  holdAttach.reset(new sliderAttach(*linkedTree, holdId, holdSlider));
  decayAttach.reset(new sliderAttach(*linkedTree, decayId, decaySlider));
  sustainAttach.reset(new sliderAttach(*linkedTree, sustainId, sustainSlider));
  releaseAttach.reset(new sliderAttach(*linkedTree, releaseId, releaseSlider));
}

EnvelopeComponent::~EnvelopeComponent() {
  // delaySlider.setLookAndFeel(nullptr);
  // attackSlider.setLookAndFeel(nullptr);
  // holdSlider.setLookAndFeel(nullptr);
  // decaySlider.setLookAndFeel(nullptr);
  // sustainSlider.setLookAndFeel(nullptr);
  // releaseSlider.setLookAndFeel(nullptr);
}

void EnvelopeComponent::resized() {
  auto bounds = getLocalBounds().toFloat();
  auto dY = bounds.getHeight() / 2.0f;
  auto upper = bounds.removeFromTop(dY);
  auto dX = upper.getWidth() / 10.0f;
  auto mBounds = upper.removeFromRight(dX);
  meter.setBounds(mBounds.toNearestInt());
  graph.setBounds(upper.toNearestInt());

  dX = bounds.getWidth() / 6.0f;
  auto labelHeight = bounds.getHeight() / 8;
  bounds.removeFromBottom(2.5f);

  auto delBounds = bounds.removeFromLeft(dX);
  delayName.setBounds(delBounds.removeFromBottom(labelHeight).toNearestInt());
  delaySlider.setBounds(delBounds.toNearestInt());

  auto aBounds = bounds.removeFromLeft(dX);
  attackName.setBounds(aBounds.removeFromBottom(labelHeight).toNearestInt());
  attackSlider.setBounds(aBounds.toNearestInt());

  auto hBounds = bounds.removeFromLeft(dX);
  holdName.setBounds(hBounds.removeFromBottom(labelHeight).toNearestInt());
  holdSlider.setBounds(hBounds.toNearestInt());

  auto dBounds = bounds.removeFromLeft(dX);
  decayName.setBounds(dBounds.removeFromBottom(labelHeight).toNearestInt());
  decaySlider.setBounds(dBounds.toNearestInt());

  auto sBounds = bounds.removeFromLeft(dX);
  sustainName.setBounds(sBounds.removeFromBottom(labelHeight).toNearestInt());
  sustainSlider.setBounds(sBounds.toNearestInt());

  auto rBounds = bounds;
  releaseName.setBounds(rBounds.removeFromBottom(labelHeight).toNearestInt());
  releaseSlider.setBounds(rBounds.toNearestInt());
}

