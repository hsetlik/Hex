//===================================================
#include "GUI/EnvelopeComponent.h"
#include "GUI/Color.h"
#include "Identifiers.h"

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
  g.fillAll(UIColor::bkgndGray);
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
  g.setColour(UIColor::greenLight);
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
      dDelay(&delaySlider),
      dAttack(&attackSlider),
      dHold(&holdSlider),
      dDecay(&decaySlider),
      dSustain(&sustainSlider),
      dRelease(&releaseSlider) {
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

  addAndMakeVisible(&dDelay);
  addAndMakeVisible(&dAttack);
  addAndMakeVisible(&dHold);
  addAndMakeVisible(&dDecay);
  addAndMakeVisible(&dSustain);
  addAndMakeVisible(&dRelease);

  // delayName.attachToComponent(&delaySlider, false);
  // attackName.attachToComponent(&attackSlider, false);
  // holdName.attachToComponent(&holdSlider, false);
  // decayName.attachToComponent(&decaySlider, false);
  // sustainName.attachToComponent(&sustainSlider, false);
  // releaseName.attachToComponent(&releaseSlider, false);

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

  auto msCallback = [](float value) {
    auto fullStr = String(value);
    // for values over 100 ms, don't bother with decimals in any case
    if (value >= 100.0f) {
      auto numberStr = fullStr.substring(0, 3);
      return numberStr + " ms";
    }
    auto pointIdx = fullStr.indexOf(".");
    int placesToUse = 0;
    bool foundZero = false;
    while (pointIdx != -1 && placesToUse < 3 && !foundZero) {
      auto decValue = fullStr[pointIdx + placesToUse];
      if (decValue == '0') {
        foundZero = true;
      }
      ++placesToUse;
    }
    if (pointIdx != -1) {
      auto numStr = fullStr.substring(0, pointIdx + placesToUse);
      return numStr + " ms";
    }
    return fullStr + " ms";
  };
  dDelay.setDisplayCallback(msCallback);
  dAttack.setDisplayCallback(msCallback);
  dHold.setDisplayCallback(msCallback);
  dDecay.setDisplayCallback(msCallback);
  dRelease.setDisplayCallback(msCallback);

  auto sustainCallback = [](float value) {
    const float fPercent = value * 100.0f;
    const int iPercent = (int)std::floorf(fPercent);
    return String(iPercent) + "%";
  };
  dSustain.setDisplayCallback(sustainCallback);
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
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 250.0f;
  const float yScale = fBounds.getHeight() / 250.0f;

  // 1. place the graph & level meter
  frect_t graphBounds = {0.0f, 0.0f, 230.0f * xScale, 130.0f * yScale};
  frect_t meterBounds = {230.0f * xScale, 0.0f, 20.0f * xScale,
                         130.0f * yScale};
  graph.setBounds(graphBounds.toNearestInt());
  meter.setBounds(meterBounds.toNearestInt());
  // 2. place the sliders
  frect_t delayBounds = {6.0f * xScale, 148.0f * yScale, 28.0f * xScale,
                         100.0f * yScale};
  frect_t attackBounds = {48.0f * xScale, 148.0f * yScale, 28.0f * xScale,
                          100.0f * yScale};
  frect_t holdBounds = {90.0f * xScale, 148.0f * yScale, 28.0f * xScale,
                        100.0f * yScale};
  frect_t decayBounds = {132.0f * xScale, 148.0f * yScale, 28.0f * xScale,
                         100.0f * yScale};
  frect_t sustainBounds = {174.0f * xScale, 148.0f * yScale, 28.0f * xScale,
                           100.0f * yScale};
  frect_t releaseBounds = {217.0f * xScale, 148.0f * yScale, 28.0f * xScale,
                           100.0f * yScale};
  delaySlider.setBounds(delayBounds.toNearestInt());
  attackSlider.setBounds(attackBounds.toNearestInt());
  holdSlider.setBounds(holdBounds.toNearestInt());
  decaySlider.setBounds(decayBounds.toNearestInt());
  sustainSlider.setBounds(sustainBounds.toNearestInt());
  releaseSlider.setBounds(releaseBounds.toNearestInt());
  // 3. place the labels
  const float dispY = 130.0f * yScale;
  const float dispHeight = 18.0f * yScale;
  const float dispWidth = fBounds.getWidth() / 6.0f;
  float xPos = 0.0f;
  frect_t delayDispBounds = {xPos, dispY, dispWidth, dispHeight};
  dDelay.setBounds(delayDispBounds.toNearestInt());
  xPos += dispWidth;
  frect_t attackDispBounds = {xPos, dispY, dispWidth, dispHeight};
  dAttack.setBounds(attackDispBounds.toNearestInt());
  xPos += dispWidth;
  frect_t holdDispBounds = {xPos, dispY, dispWidth, dispHeight};
  dHold.setBounds(holdDispBounds.toNearestInt());
  xPos += dispWidth;
  frect_t decayDispBounds = {xPos, dispY, dispWidth, dispHeight};
  dDecay.setBounds(decayDispBounds.toNearestInt());
  xPos += dispWidth;
  frect_t sustainDispBounds = {xPos, dispY, dispWidth, dispHeight};
  dSustain.setBounds(sustainDispBounds.toNearestInt());
  xPos += dispWidth;
  frect_t releaseDispBounds = {xPos, dispY, dispWidth, dispHeight};
  dRelease.setBounds(releaseDispBounds.toNearestInt());
}

