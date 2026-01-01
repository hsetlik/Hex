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
#include "MathUtil.h"
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
  SliderUtil::setRotaryNoBox(levelSlider);

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
  fBounds = fBounds.reduced(OPERATOR_INSET);
  const float x0 = fBounds.getX();
  const float y0 = fBounds.getY();
  auto envBounds = fBounds.removeFromBottom(fBounds.getHeight() / 2.0f);
  envComponent.setBounds(envBounds.toNearestInt());
  auto dX = fBounds.getWidth() / 25.0f;
  auto dY = fBounds.getHeight() / 16.0f;
  const float sRatio = 3.5f;
  frect_t ratioBox = {x0 + dX, y0 + dX * 5.0f, dX * sRatio, dX * sRatio};
  frect_t modBounds = {x0 + dX * 6.0f, y0 + dX * 5.0f, dX * sRatio,
                       dX * sRatio};
  frect_t levelBounds = {x0 + dX * 11.0f, y0 + dX * 5.0f, dX * sRatio,
                         dX * sRatio};
  frect_t panBounds = {x0 + dX * 16.0f, y0 + dX * 5.0f, dX * sRatio,
                       dX * sRatio};
  frect_t outBounds = {x0 + dX * 16.0f, y0 + dX, dX * 6.0f, dX * 2.0f};
  frect_t selectBounds = {x0 + dX, y0 + dY * 12.5f, dX * 16.0f, dY * 3.0f};

  ratioSlider.setBounds(ratioBox.toNearestInt());
  modSlider.setBounds(modBounds.toNearestInt());
  levelSlider.setBounds(levelBounds.toNearestInt());
  panSlider.setBounds(panBounds.toNearestInt());
  outButton.setBounds(outBounds.toNearestInt());
  waveSelect.setBounds(selectBounds.toNearestInt());
}

void OperatorComponent::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UXPalette::darkGray);
  g.fillRect(fBounds);
  fBounds = fBounds.reduced(OPERATOR_INSET / 2.0f);
  g.setColour(UXPalette::lightGray);
  g.fillRect(fBounds);
  auto upperBox = fBounds.removeFromTop(25.0f);
  auto tBounds = upperBox.removeFromLeft(120.0f);
  tBounds.removeFromLeft(3.0f);
  const String text = "Operator " + String(opIndex + 1);
  AttString aStr(text);
  aStr.setJustification(juce::Justification::centredLeft);
  aStr.setFont(Fonts::getFont(Fonts::RobotoBlackItalic, 22.0f));
  aStr.setColour(juce::Colours::white);
  aStr.draw(g, tBounds);
}
