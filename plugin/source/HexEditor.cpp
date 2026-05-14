/*
  ==============================================================================

    HexEditor.cpp
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "GUI/HexEditor.h"
#include "GUI/Color.h"
#include "GUI/Fonts.h"
#include "Identifiers.h"
#include "juce_core/juce_core.h"
#include "GUI/ComponentUtil.h"
#include "juce_graphics/juce_graphics.h"
FilterPanel::FilterPanel(HexState* tree, GraphParamSet* graph)
    : linkedTree(&tree->mainTree),
      envComp(0, linkedTree, graph, true),
      cutoffName("Cutoff"),
      resName("Resonance"),
      wetName("Mix"),
      depthName("Env. depth") {
  cutoffAttach.reset(
      new sliderAttach(*linkedTree, ID::filterCutoff.toString(), cutoffSlider));
  resAttach.reset(
      new sliderAttach(*linkedTree, ID::filterResonance.toString(), resSlider));
  wetAttach.reset(
      new sliderAttach(*linkedTree, ID::filterWetDry.toString(), wetSlider));
  depthAttach.reset(new sliderAttach(*linkedTree, ID::filterEnvDepth.toString(),
                                     depthSlider));

  typeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
      *linkedTree, ID::filterType.toString(), typeBox));
  typeBox.addItem("None", 1);
  typeBox.addItem("Low Pass", 2);
  typeBox.addItem("High Pass", 3);
  typeBox.addItem("Band Pass", 4);
  typeBox.setSelectedId(1);

  addAndMakeVisible(&typeBox);

  SliderUtil::setRotaryNoBox(cutoffSlider);
  SliderUtil::setRotaryNoBox(resSlider);
  SliderUtil::setRotaryNoBox(wetSlider);
  SliderUtil::setRotaryNoBox(depthSlider);

  addAndMakeVisible(&envComp);
  addAndMakeVisible(&cutoffSlider);
  addAndMakeVisible(&resSlider);
  addAndMakeVisible(&wetSlider);
  addAndMakeVisible(&depthSlider);

  addAndMakeVisible(&cutoffName);
  addAndMakeVisible(&resName);
  addAndMakeVisible(&wetName);
  addAndMakeVisible(&depthName);

  cutoffName.setJustificationType(juce::Justification::centred);
  resName.setJustificationType(juce::Justification::centred);
  wetName.setJustificationType(juce::Justification::centred);
  depthName.setJustificationType(juce::Justification::centred);
  cutoffName.attachToComponent(&cutoffSlider, false);
  resName.attachToComponent(&resSlider, false);
  wetName.attachToComponent(&wetSlider, false);
  depthName.attachToComponent(&depthSlider, false);
}

FilterPanel::~FilterPanel() {}

void FilterPanel::resized() {
  // auto bounds = getLocalBounds();
  // auto sWidth = bounds.getWidth() / 4;
  // auto upperBounds = bounds.removeFromTop(sWidth);
  // auto midBounds = bounds.removeFromTop(sWidth);
  // auto cushion = midBounds.getHeight() / 4;
  // typeBox.setBounds(midBounds.reduced(cushion));
  // cushion /= 2;
  // cutoffSlider.setBounds(
  //     upperBounds.removeFromLeft(sWidth).reduced(cushion).withY(2 * cushion));
  // resSlider.setBounds(
  //     upperBounds.removeFromLeft(sWidth).reduced(cushion).withY(2 * cushion));
  // wetSlider.setBounds(
  //     upperBounds.removeFromLeft(sWidth).reduced(cushion).withY(2 * cushion));
  // depthSlider.setBounds(upperBounds.reduced(cushion).withY(2 * cushion));
  //envComp.setBounds(bounds.reduced(4));

  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 446.0f;
  const float yScale = fBounds.getHeight() / 435.0f;
  const float minScale = std::min(xScale, yScale);
  // positions for the knobs and type box
  frect_t cutoffBounds = {27.0f * xScale, 56.0f * yScale, 40.0f * minScale, 40.0f * minScale};
  frect_t resBounds = {107.0f * xScale, 56.0f * yScale, 40.0f * minScale, 40.0f * minScale};
  frect_t depthBounds = {187.0f * xScale, 56.0f * yScale, 40.0f * minScale, 40.0f * minScale};
  frect_t wetBounds = {267.0f * xScale, 56.0f * yScale, 40.0f * minScale, 40.0f * minScale};
  frect_t typeBounds = {317.0f * xScale, 62.0f * yScale, 105.0f * xScale, 20.0f * yScale};
  cutoffSlider.setBounds(cutoffBounds.toNearestInt());
  resSlider.setBounds(resBounds.toNearestInt());
  depthSlider.setBounds(depthBounds.toNearestInt());
  wetSlider.setBounds(wetBounds.toNearestInt());
  typeBox.setBounds(typeBounds.toNearestInt());
  // position the envelope component
  frect_t envBounds = {20.0f * xScale, 106.0f * yScale, 400.0f * xScale, 328.0f * yScale};
  envComp.setBounds(envBounds.toNearestInt());
}

void FilterPanel::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 446.0f;
  const float yScale = fBounds.getHeight() / 435.0f;
  g.setColour(UIColor::shadowGray);
  g.fillRect(fBounds);

  frect_t txtBounds = {191.0f * xScale, 3.0f * yScale, 48.0f * xScale, 26.0f * yScale};
  auto font = Fonts::getFont(Fonts::KenyanReg, 26.0f * yScale);
  AttString aStr("Filter");
  aStr.setFont(font);
  aStr.setJustification(juce::Justification::centred);
  aStr.setColour(UIColor::orangeLight);
  aStr.draw(g, txtBounds);
}
//==============================================================================

BottomBar::BottomBar(apvts* tree, juce::MidiKeyboardState& kbdState)
    : velName("Velocity tracking"),
      kbdComp(kbdState, juce::KeyboardComponentBase::horizontalKeyboard) {
  addAndMakeVisible(velSlider);
  addAndMakeVisible(velName);
  addAndMakeVisible(kbdComp);
  velName.attachToComponent(&velSlider, false);
  velName.setVerticalScale(0.6f);
  SliderUtil::setRotaryNoBox(velSlider);
  velAttach.reset(
      new sliderAttach(*tree, ID::velocityTracking.toString(), velSlider));
}

void BottomBar::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto vBounds = fBounds.removeFromLeft(fBounds.getHeight());
  velSlider.setBounds(vBounds.reduced(18.0f).toNearestInt());
  kbdComp.setBounds(fBounds.toNearestInt());
}

void BottomBar::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UXPalette::lightGray);
  g.fillRect(fBounds);
}

//==============================================================================
HexEditor::HexEditor(HexAudioProcessor* proc,
                     HexState* tree,
                     GraphParamSet* params,
                     RingBuffer<float>* buffer,
                     juce::MidiKeyboardState& kbdState)
    : linkedTree(&tree->mainTree),
      modGrid(linkedTree),
      graph(params, buffer),
      fPanel(tree, params),
      kbdBar(linkedTree, kbdState),
      upperBar(tree),
      saveDialog(tree),
      loadDialog(tree) {
  setLookAndFeel(&lnf);
  addAndMakeVisible(&modGrid);
  nonModalComps.push_back(&modGrid);
  addAndMakeVisible(&graph);

  nonModalComps.push_back(&graph);
  addAndMakeVisible(&fPanel);
  nonModalComps.push_back(&fPanel);
  addAndMakeVisible(&kbdBar);
  nonModalComps.push_back(&kbdBar);
  addAndMakeVisible(&upperBar);
  nonModalComps.push_back(&upperBar);
  for (int i = 0; i < NUM_OPERATORS; ++i) {
    addAndMakeVisible(
        opComponents.add(new OperatorComponent(i, linkedTree, params)));
    nonModalComps.push_back(opComponents.getLast());
  }
  for (int i = 0; i < NUM_LFOS; ++i) {
    addAndMakeVisible(
        lfoComponents.add(new LfoComponent(i, proc, params, linkedTree)));
    nonModalComps.push_back(lfoComponents.getLast());
  }
  addAndMakeVisible(saveDialog);
  saveDialog.setVisible(false);
  saveDialog.setEnabled(false);
  addAndMakeVisible(loadDialog);
  loadDialog.setVisible(false);
  loadDialog.setEnabled(false);
}

HexEditor::~HexEditor() {
  setLookAndFeel(nullptr);
}

void HexEditor::resized() {
  const float xScale = (float)getWidth() / 1800.0f;
  const float yScale = (float)getHeight() / 1080.0f;
  const float dWidth = (float)getWidth() / 4.5f;
  auto gridWidth = dWidth;
  auto bounds = getLocalBounds();
  auto kBounds = bounds.removeFromBottom(100);
  kbdBar.setBounds(kBounds);

  auto upperBarBounds = bounds.removeFromTop((int)(100.0f * yScale));
  upperBar.setBounds(upperBarBounds);
  auto rightColumn = bounds.removeFromRight((int)gridWidth).toFloat();
  resizedRightColumn(rightColumn);

  auto lfoBounds = bounds.removeFromBottom(bounds.getHeight() / 5);
  auto lfoWidth = lfoBounds.getWidth() / NUM_LFOS;
  for (int i = 0; i < NUM_LFOS - 1; ++i) {
    lfoComponents[i]->setBounds(lfoBounds.removeFromLeft(lfoWidth));
  }
  lfoComponents[NUM_LFOS - 1]->setBounds(lfoBounds);
  auto dX = bounds.getWidth() / 3;
  auto topBounds = bounds.removeFromTop(bounds.getHeight() / 2);
  opComponents[0]->setBounds(topBounds.removeFromLeft(dX));
  opComponents[1]->setBounds(topBounds.removeFromLeft(dX));
  opComponents[2]->setBounds(topBounds);

  opComponents[3]->setBounds(bounds.removeFromLeft(dX));
  opComponents[4]->setBounds(bounds.removeFromLeft(dX));
  opComponents[5]->setBounds(bounds);

  auto xCushion = getWidth() / 3.5;
  auto yCushion = getHeight() / 3.0;
  auto saveBounds = getLocalBounds().reduced((int)xCushion, (int)yCushion);
  saveDialog.setBounds(saveBounds);
  loadDialog.setBounds(saveBounds);
}


void HexEditor::resizedRightColumn(frect_t& bounds){
  const float x0 = bounds.getX();
  const float y0 = bounds.getY();
  const float xScale = bounds.getWidth() / 446.0f;
  const float yScale = bounds.getHeight() / 980.0f;
  frect_t gridOuter = {x0, y0, 446.0f * xScale, 324.0f * yScale};
  modGrid.setBounds(gridOuter.toNearestInt());

  frect_t graphBounds = {x0 + 14.0f * xScale, y0 + 334.0f * yScale, 420.0f * xScale, 190.0f * yScale};
  graph.setBounds(graphBounds.toNearestInt());

  frect_t filtBounds = {x0, y0 + 545.0f * yScale, 446.0f * xScale, 435.0f * yScale};
  fPanel.setBounds(filtBounds.toNearestInt());
  // const float minScale = std::fmin(xScale, yScale);
  // auto gridBounds = bounds.removeFromTop(minScale * 280.0f);
  // modGrid.setBounds(gridBounds.toNearestInt());

  // auto graphBounds = bounds.removeFromTop(195.0f * yScale);
  // graphBounds = graphBounds.withSizeKeepingCentre(420.0f * xScale, 178.0f * yScale);
  // graph.setBounds(graphBounds.toNearestInt());

  // auto filterBounds = bounds.withSizeKeepingCentre(400.0f * xScale, 475.0f * yScale);
  // fPanel.setBounds(filterBounds.toNearestInt());
}

void HexEditor::paint(juce::Graphics& g) {
  g.setColour(UIColor::shadowGray);
  g.fillAll();
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = (float)getWidth() / 1800.0f;
  const float yScale = (float)getHeight() / 1080.0f;
  auto graphBounds = graph.getBoundsInParent().toFloat();
  const float barY1 = graphBounds.getY() - (8.0f * yScale);
  frect_t bar1Bounds = {graphBounds.getX(), barY1, graphBounds.getWidth(), 4.0f * yScale};
  g.setColour(UIColor::borderGray);
  g.fillRoundedRectangle(bar1Bounds, 2.0f * yScale);
  const float barY2 = graphBounds.getBottom() + (4.0f * yScale);
  frect_t bar2Bounds = {graphBounds.getX(), barY2, graphBounds.getWidth(), 4.0f * yScale};
  g.fillRoundedRectangle(bar2Bounds, 2.0f * yScale);
}

//------------------------------------------------------

void HexEditor::setNonModalsEnabled(bool enabled) {
  for (auto* c : nonModalComps) {
    c->setEnabled(enabled);
  }
}

void HexEditor::openSaveDialog(const String& patchName) {
  setNonModalsEnabled(false);
  saveDialog.initializeFor(patchName);
  saveDialog.setEnabled(true);
  saveDialog.setVisible(true);
  saveDialog.toFront(true);
  resized();
}

void HexEditor::openLoadDialog(const String& patchName) {
  setNonModalsEnabled(false);
  loadDialog.initializeFor(patchName);
  loadDialog.setEnabled(true);
  loadDialog.setVisible(true);
  loadDialog.toFront(true);
  resized();
}

void HexEditor::closeModal() {
  saveDialog.setEnabled(false);
  saveDialog.setVisible(false);
  loadDialog.setEnabled(false);
  loadDialog.setVisible(false);
  setNonModalsEnabled(true);
  resized();
}
