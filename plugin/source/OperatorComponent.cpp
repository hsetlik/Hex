/*
  ==============================================================================

    OperatorComponent.cpp
    Created: 7 Jun 2021 2:24:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/OperatorComponent.h"
#include "GUI/Assets.h"
#include "GUI/Color.h"
#include "GUI/Fonts.h"
#include "Identifiers.h"
#include "GUI/ComponentUtil.h"
#include "juce_graphics/juce_graphics.h"
#define OPERATOR_INSET 4.0f
//=======================================================
OutputToggle::OutputToggle() : juce::Button("OutputToggle") {}

void OutputToggle::paintButton(juce::Graphics& g, bool highlighted, bool down) {
  juce::ignoreUnused(highlighted, down);
  auto fBounds = getLocalBounds().toFloat();
  const float sideLength = std::min(fBounds.getHeight(), fBounds.getWidth());
  auto btnBounds = fBounds.withSizeKeepingCentre(sideLength, sideLength);
  auto& img = getToggleState() ? Assets::getImage(Assets::PlaybackOn)
                               : Assets::getImage(Assets::PlaybackOff);
  g.drawImage(img, btnBounds);
}
//=======================================================
OperatorComponent::OperatorComponent(int idx,
                                     apvts* tree,
                                     GraphParamSet* gParams)
    : opIndex(idx),
      linkedTree(tree),
      envComponent(idx, tree, gParams),
      waveSelect(*tree, ID::operatorWaveShape.toString() + String(idx)),
      ratioName("Ratio"),
      modName("Mod Index"),
      panName("Pan"),
      levelName("Level"),
      ratioLabel(&ratioSlider),
      modLabel(&modSlider),
      panLabel(&panSlider),
      levelLabel(&levelSlider) {
  // setLookAndFeel(&hexLnf);
  addAndMakeVisible(&envComponent);
  addAndMakeVisible(&waveSelect);

  SliderUtil::setRotaryNoBox(ratioSlider);
  SliderUtil::setRotaryNoBox(modSlider);
  SliderUtil::setRotaryNoBox(panSlider);
  // SliderUtil::setRotaryNoBox(levelSlider);
  levelSlider.setSliderStyle(juce::Slider::LinearVertical);
  levelSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

  addAndMakeVisible(&ratioSlider);
  addAndMakeVisible(&modSlider);
  addAndMakeVisible(&panSlider);
  addAndMakeVisible(&levelSlider);

  ratioName.attachToComponent(&ratioSlider, false);
  modName.attachToComponent(&modSlider, false);
  levelName.attachToComponent(&levelSlider, false);
  panName.attachToComponent(&panSlider, false);

  addAndMakeVisible(&ratioName);
  addAndMakeVisible(&modName);
  addAndMakeVisible(&panName);
  addAndMakeVisible(&levelName);

  outButton.setClickingTogglesState(true);
  addAndMakeVisible(&outButton);

  addAndMakeVisible(&ratioLabel);
  addAndMakeVisible(&modLabel);
  addAndMakeVisible(&panLabel);
  addAndMakeVisible(&levelLabel);

  ratioLabel.attachToComponent(&ratioSlider, false);
  modLabel.attachToComponent(&modSlider, false);
  panLabel.attachToComponent(&panSlider, false);
  levelLabel.attachToComponent(&levelSlider, false);

  auto iStr = juce::String(opIndex);
  auto ratioId = ID::operatorRatio + iStr;
  auto indexId = ID::operatorModIndex + iStr;
  auto panId = ID::operatorPan + iStr;
  auto levelId = ID::operatorLevel + iStr;
  auto outputId = ID::operatorAudible + iStr;

  ratioAttach.reset(new sliderAttach(*linkedTree, ratioId, ratioSlider));
  modAttach.reset(new sliderAttach(*linkedTree, indexId, modSlider));
  panAttach.reset(new sliderAttach(*linkedTree, panId, panSlider));
  levelAttach.reset(new sliderAttach(*linkedTree, levelId, levelSlider));

  outAttach.reset(new buttonAttach(*linkedTree, outputId, outButton));

  outButton.addListener(this);
  buttonClicked(&outButton);
}

OperatorComponent::~OperatorComponent() {
  // setLookAndFeel(nullptr);
}

void OperatorComponent::buttonClicked(juce::Button* b) {
  if (b->getToggleState()) {
    panSlider.setVisible(true);
    panLabel.setVisible(true);
  } else {
    panSlider.setVisible(false);
    panLabel.setVisible(false);
  }
  outButton.repaint();
}

void OperatorComponent::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 450.0f;
  const float yScale = fBounds.getHeight() / 365.0f;

  frect_t waveSelBounds = {10.0f * xScale, 60.0f * yScale, 180.0f * xScale,
                           36.0f * yScale};
  waveSelect.setBounds(waveSelBounds.toNearestInt());

  frect_t envBounds = {9.0f * xScale, 108.0f * yScale, 250.0f * xScale,
                       249.0f * yScale};
  envComponent.setBounds(envBounds.toNearestInt());

  frect_t outBounds = {300.0f * xScale, 40.0f * yScale, 40.0f * xScale,
                       40.0f * yScale};
  frect_t ratioBounds = {300.0f * xScale, 128.0f * yScale, 40.0f * xScale,
                         40.0f * yScale};
  frect_t panBounds = {385.0f * xScale, 40.0f * yScale, 40.0f * xScale,
                       40.0f * yScale};
  frect_t indexBounds = {385.0f * xScale, 128.0f * yScale, 40.0f * xScale,
                         40.0f * yScale};

  outButton.setBounds(outBounds.toNearestInt());
  ratioSlider.setBounds(ratioBounds.toNearestInt());
  panSlider.setBounds(panBounds.toNearestInt());
  modSlider.setBounds(indexBounds.toNearestInt());

  frect_t levelBounds = {346.0f * xScale, 231.0f * yScale, 28.0f * xScale,
                         130.0f * yScale};
  levelSlider.setBounds(levelBounds.toNearestInt());
}

void OperatorComponent::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 450.0f;
  const float yScale = fBounds.getHeight() / 365.0f;
  // draw the background
  g.setColour(UIColor::borderGray);
  g.fillRect(fBounds);
  auto innerBounds =
      fBounds.withSizeKeepingCentre(445.0f * xScale, 361.0f * yScale);
  g.setColour(UIColor::shadowGray);
  g.fillRect(innerBounds);
  // draw the operator label
  String labelTxt = "OPERATOR " + String(opIndex + 1);
  AttString aStr(labelTxt);
  aStr.setColour(UIColor::orangeLight);
  aStr.setFont(
      Fonts::getFont(Fonts::KenyanBoldItalic).withHeight(40.0f * yScale));
  aStr.setWordWrap(AttString::none);
  aStr.setJustification(juce::Justification::centredLeft);
  auto txtBounds = juce::TextLayout::getStringBounds(aStr);
  txtBounds.setPosition(10.0f * xScale, 8.0f * yScale);
  aStr.draw(g, txtBounds);

  // draw the little divider things
  const float divWidth = 4.0f * std::min(xScale, yScale);
  frect_t div1Bounds = {12.0f * xScale, 46.0f * yScale, 176 * xScale, divWidth};
  frect_t div2Bounds = {270.0f * xScale, 111.0f * yScale, divWidth,
                        250.0f * yScale};
  g.setColour(UIColor::borderGray);
  g.fillRoundedRectangle(div1Bounds, divWidth * 0.5f);
  g.fillRoundedRectangle(div2Bounds, divWidth * 0.5f);
}
