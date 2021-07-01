/*
  ==============================================================================

    OperatorComponent.cpp
    Created: 7 Jun 2021 2:24:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "OperatorComponent.h"

EnvelopeComponent::DAHDSRGraph::DAHDSRGraph(EnvelopeComponent* env) :
pDelay(&env->delaySlider),
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
needsRepaint(false)
{
    startTimerHz(REPAINT_FPS);
}
void EnvelopeComponent::DAHDSRGraph::timerCallback()
{
    if(needsRepaint)
    {
        repaint();
        needsRepaint = false;
    }
}
void EnvelopeComponent::DAHDSRGraph::sliderValueChanged(juce::Slider *slider)
{
    if(slider == pDelay)
    {
        delayVal = slider->getValue();
        needsRepaint = true;
    }
    else if(slider == pAttack)
    {
        attackVal = slider->getValue();
        needsRepaint = true;
    }
    else if(slider == pHold)
    {
        holdVal = slider->getValue();
        needsRepaint = true;
    }
    else if(slider == pDecay)
    {
        decayVal = slider->getValue();
        needsRepaint = true;
    }
    else if(slider == pSustain)
    {
        sustainVal = slider->getValue();
        needsRepaint = true;
    }
    else if(slider == pRelease)
    {
        releaseVal = slider->getValue();
        needsRepaint = true;
    }
}
void EnvelopeComponent::DAHDSRGraph::paint(juce::Graphics &g)
{
    g.fillAll(UXPalette::darkGray);
    auto area = getLocalBounds().toFloat();
    auto timeTotal = delayVal + attackVal + holdVal + decayVal + releaseVal;
    juce::Path trace;
    trace.startNewSubPath(0.0f, area.getHeight());
    trace.lineTo(delayVal, area.getHeight());
    trace.lineTo(delayVal + attackVal, 0.0f);
    trace.lineTo(delayVal + attackVal + holdVal, 0.0f);
    auto sustainY = (1.0f - sustainVal) * area.getHeight();
    
    auto sustainLength = timeTotal * 0.25;
    trace.lineTo(delayVal + attackVal + holdVal + decayVal, sustainY);
    trace.lineTo(delayVal + attackVal + holdVal + decayVal + sustainLength, sustainY);
    trace.lineTo(timeTotal + sustainLength, area.getHeight());
    trace.scaleToFit(0.0f, 5.0f, area.getWidth(), (area.getHeight() - 5.0f), false);
    
    auto stroke = juce::PathStrokeType(1.0f);
    g.setColour(UXPalette::highlight);
    g.strokePath(trace, stroke);
}
//==============================================================================
EnvelopeComponent::LevelMeter::LevelMeter(int idx, GraphParamSet* params, bool filter) :
envIndex(idx),
isFilter(filter),
linkedParams(params),
level(0.0f),
lastVoice(0)
{
    startTimerHz(REPAINT_FPS);
}

void EnvelopeComponent::LevelMeter::timerCallback() {triggerAsyncUpdate(); }

void EnvelopeComponent::LevelMeter::handleAsyncUpdate()
{
    lastVoice = linkedParams->lastTriggeredVoice.load();
    float newLevel = 0.0f;
    if(linkedParams->filterLevels[lastVoice].is_lock_free())
    {
        if(!isFilter)
            newLevel = linkedParams->levels[lastVoice][envIndex].load();
        else
            newLevel = linkedParams->filterLevels[lastVoice].load();
    }
    else
        printf("Warning: Graph parameters have locks!\n");
    if(level != newLevel)
    {
        level = newLevel;
        repaint();
    }
}

void EnvelopeComponent::LevelMeter::paint(juce::Graphics &g)
{
    auto bBounds = getLocalBounds().toFloat();
    auto lBounds = juce::Rectangle<float>(bBounds.getX(), bBounds.getY(), bBounds.getWidth(), bBounds.getHeight() * (1.0f - level));
    g.setColour(UXPalette::highlight);
    g.fillRect(bBounds);
    g.setColour(UXPalette::darkBlue);
    g.fillRect(lBounds);
}
//asdferasdfasdf
//==============================================================================
EnvelopeComponent::EnvelopeComponent(int idx, apvts* tree, GraphParamSet* gParams, bool isFilterComp) :
opIndex(idx),
isFilter(isFilterComp),
linkedTree(tree),
graph(this),
meter(idx, gParams, isFilter)
{
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
    
    delaySlider.addListener(&graph);
    attackSlider.addListener(&graph);
    holdSlider.addListener(&graph);
    decaySlider.addListener(&graph);
    sustainSlider.addListener(&graph);
    releaseSlider.addListener(&graph);
    
    addAndMakeVisible(&graph);
    
    addAndMakeVisible(&meter);
    
    auto iStr = juce::String(opIndex);
    auto delayId = "delayParam" + iStr;
    auto attackId = "attackParam" + iStr;
    auto holdId = "holdParam" + iStr;
    auto decayId = "decayParam" + iStr;
    auto sustainId = "sustainParam" + iStr;
    auto releaseId = "releaseParam" + iStr;
    if(isFilter)
    {
        delayId = "filterDelayParam";
        attackId = "filterAttackParam";
        holdId = "filterHoldParam";
        decayId = "filterDecayParam";
        sustainId = "filterSustainParam";
        releaseId = "filterReleaseParam";
    }
    
    delayAttach.reset(new sliderAttach(*linkedTree, delayId, delaySlider));
    attackAttach.reset(new sliderAttach(*linkedTree, attackId, attackSlider));
    holdAttach.reset(new sliderAttach(*linkedTree, holdId, holdSlider));
    decayAttach.reset(new sliderAttach(*linkedTree, decayId, decaySlider));
    sustainAttach.reset(new sliderAttach(*linkedTree, sustainId, sustainSlider));
    releaseAttach.reset(new sliderAttach(*linkedTree, releaseId, releaseSlider));
}

void EnvelopeComponent::resized()
{
    auto bounds = getLocalBounds();
    auto dY = bounds.getHeight() / 2;
    auto upper = bounds.removeFromTop(dY);
    auto dX = upper.getWidth() / 10;
    auto mBounds = upper.removeFromRight(dX);
    meter.setBounds(mBounds);
    graph.setBounds(upper);
    dX = bounds.getWidth() / 6;
    delaySlider.setBounds(bounds.removeFromLeft(dX));
    attackSlider.setBounds(bounds.removeFromLeft(dX));
    holdSlider.setBounds(bounds.removeFromLeft(dX));
    decaySlider.setBounds(bounds.removeFromLeft(dX));
    sustainSlider.setBounds(bounds.removeFromLeft(dX));
    releaseSlider.setBounds(bounds);
}
//==============================================================================
void SineButton::setSymbol()
{
    const int resolution = 65;
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto y0 = bounds.getY() + (bounds.getHeight() / 2.0f);
    auto amplitude = bounds.getHeight() / 2.0f;
    auto x0 = bounds.getX();
    auto dX = bounds.getWidth() / (float)resolution;
    auto dAngle = juce::MathConstants<float>::twoPi / (float)resolution;
    symbol.clear();
    symbol.startNewSubPath(x0, y0);
    for(int i = 0; i < resolution; ++i)
    {
        auto x = x0 + (i * dX);
        auto y = y0 + (amplitude * std::sin(dAngle * i));
        symbol.lineTo(x, y);
    }
}
void SquareButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto dX = bounds.getWidth() / 4;
    auto x0 = bounds.getX();
    auto y0 = bounds.getBottom();
    auto xEnd = bounds.getRight();
    auto yMax = bounds.getY();
    symbol.clear();
    symbol.startNewSubPath(x0, y0);
    symbol.lineTo(x0 + dX, y0);
    symbol.lineTo(x0 + dX, yMax);
    symbol.lineTo(x0 + (3 * dX), yMax);
    symbol.lineTo(x0 + (3 * dX), y0);
    symbol.lineTo(xEnd, y0);
}
void SawButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto xMin = bounds.getX();
    auto xMax = bounds.getRight();
    auto xCenter = bounds.getX() + (bounds.getWidth() / 2.0f);
    auto yCenter = bounds.getY() + (bounds.getHeight() / 2.0f);
    auto yMin = bounds.getY();
    auto yMax = bounds.getBottom();
    symbol.clear();
    symbol.startNewSubPath(xMin, yCenter);
    symbol.lineTo(xCenter, yMin);
    symbol.lineTo(xCenter, yMax);
    symbol.lineTo(xMax, yCenter);
}
void TriButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto xMin = bounds.getX();
    auto xMax = bounds.getRight();
    auto xCenter = bounds.getX() + (bounds.getWidth() / 2.0f);
    auto yMin = bounds.getY();
    auto yMax = bounds.getBottom();
    symbol.clear();
    symbol.startNewSubPath(xMin, yMax);
    symbol.lineTo(xCenter, yMin);
    symbol.lineTo(xMax, yMax);
}
void NoiseButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto numPoints = 12;
    auto x0 = bounds.getX();
    auto y0 = bounds.getY() + bounds.getHeight() / 2.0f;
    auto amplitude = bounds.getHeight() / 2.0f;
    auto dX = bounds.getWidth() / numPoints;
    auto rand = juce::Random(NOISE_SEED);
    symbol.clear();
    symbol.startNewSubPath(x0, y0);
    for(int i = 0; i < numPoints; ++i)
    {
        auto val = (rand.nextFloat() - 0.5f) * 2.0f;
        auto x = x0 + dX * i;
        auto y = y0 + val * amplitude;
        symbol.lineTo(x, y);
    }
}
//=======================================================
WaveSelector::WaveSelector(int index, apvts* tree, juce::String prefix) :
opIndex(index),
linkedTree(tree)
{
    addAndMakeVisible(&bSine);
    addAndMakeVisible(&bSquare);
    addAndMakeVisible(&bSaw);
    addAndMakeVisible(&bTri);
    addAndMakeVisible(&bNoise);
    addAndMakeVisible(hiddenBox);
    hiddenBox.setVisible(false);
    hiddenBox.addItem("Sine", 1);
    hiddenBox.addItem("Square", 2);
    hiddenBox.addItem("Saw", 3);
    hiddenBox.addItem("Tri", 4);
    hiddenBox.addItem("Noise", 5);
    auto waveId = prefix + juce::String(opIndex);
    hiddenBoxAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(*linkedTree, waveId, hiddenBox));
    auto radioNum = 60 + opIndex;
    bSine.setRadioGroupId(radioNum);
    bSquare.setRadioGroupId(radioNum);
    bSaw.setRadioGroupId(radioNum);
    bTri.setRadioGroupId(radioNum);
    bNoise.setRadioGroupId(radioNum);
    
    bSine.triggerClick();
    
    bSine.addListener(this);
    bSquare.addListener(this);
    bSaw.addListener(this);
    bTri.addListener(this);
    bNoise.addListener(this);
}

void WaveSelector::resized()
{
    auto fBounds = getLocalBounds();
    auto dX = fBounds.getWidth() / 5;
    bSine.setBounds(fBounds.removeFromLeft(dX));
    bSquare.setBounds(fBounds.removeFromLeft(dX));
    bSaw.setBounds(fBounds.removeFromLeft(dX));
    bTri.setBounds(fBounds.removeFromLeft(dX));
    bNoise.setBounds(fBounds);
}
void WaveSelector::buttonClicked(juce::Button *b)
{
    if(b == &bSine)
        hiddenBox.setSelectedId(1);
    else if(b == &bSquare)
        hiddenBox.setSelectedId(2);
    else if(b == &bSaw)
        hiddenBox.setSelectedId(3);
    else if(b == &bTri)
        hiddenBox.setSelectedId(4);
    else if(b == &bNoise)
        hiddenBox.setSelectedId(5);
}

//=======================================================
OperatorComponent::OperatorComponent(int idx, apvts* tree, GraphParamSet* gParams) :
opIndex(idx),
linkedTree(tree),
envComponent(idx, tree, gParams),
waveSelect(idx, tree, "waveParam"),
ratioName("Ratio"),
modName("Mod Index"),
panName("Pan"),
levelName("Level"),
ratioLabel(&ratioSlider),
modLabel(&modSlider),
panLabel(&panSlider),
levelLabel(&levelSlider)
{
    addAndMakeVisible(&envComponent);
    addAndMakeVisible(&waveSelect);
    
    SliderUtil::setRotaryNoBox(ratioSlider);
    SliderUtil::setRotaryNoBox(modSlider);
    SliderUtil::setRotaryNoBox(panSlider);
    SliderUtil::setRotaryNoBox(levelSlider);
    
    ratioSlider.setLookAndFeel(&hexLnf);
    modSlider.setLookAndFeel(&hexLnf);
    panSlider.setLookAndFeel(&hexLnf);
    levelSlider.setLookAndFeel(&hexLnf);
    
    addAndMakeVisible(&ratioSlider);
    addAndMakeVisible(&modSlider);
    addAndMakeVisible(&panSlider);
    addAndMakeVisible(&levelSlider);
    
    addAndMakeVisible(&ratioName);
    addAndMakeVisible(&modName);
    addAndMakeVisible(&panName);
    addAndMakeVisible(&levelName);
    
    addAndMakeVisible(&outButton);
    
    addAndMakeVisible(&ratioLabel);
    addAndMakeVisible(&modLabel);
    addAndMakeVisible(&panLabel);
    addAndMakeVisible(&levelLabel);
    
    ratioLabel.setLookAndFeel(&hexLnf);
    modLabel.setLookAndFeel(&hexLnf);
    panLabel.setLookAndFeel(&hexLnf);
    levelLabel.setLookAndFeel(&hexLnf);
    
    auto iStr = juce::String(opIndex);
    auto ratioId = "ratioParam" + iStr;
    auto indexId = "indexParam" + iStr;
    auto panId = "panParam" + iStr;
    auto levelId = "levelParam" + iStr;
    auto outputId = "audibleParam" + iStr;
    
    ratioAttach.reset(new sliderAttach(*linkedTree, ratioId, ratioSlider));
    modAttach.reset(new sliderAttach(*linkedTree, indexId, modSlider));
    panAttach.reset(new sliderAttach(*linkedTree, panId, panSlider));
    levelAttach.reset(new sliderAttach(*linkedTree, levelId, levelSlider));
    
    outAttach.reset(new buttonAttach(*linkedTree, outputId, outButton));
    
    outButton.addListener(this);
    buttonClicked(&outButton);
}

OperatorComponent::~OperatorComponent()
{
    ratioSlider.setLookAndFeel(nullptr);
    modSlider.setLookAndFeel(nullptr);
    panSlider.setLookAndFeel(nullptr);
    levelSlider.setLookAndFeel(nullptr);
    ratioLabel.setLookAndFeel(nullptr);
    modLabel.setLookAndFeel(nullptr);
    panLabel.setLookAndFeel(nullptr);
    levelLabel.setLookAndFeel(nullptr);
}

void OperatorComponent::buttonClicked(juce::Button *b)
{
    if(b->getToggleState())
    {
        panSlider.setVisible(true);
        panLabel.setVisible(true);
    }
    else
    {
        panSlider.setVisible(false);
        panLabel.setVisible(false);
    }
}

void OperatorComponent::resized()
{
    auto fBounds = getLocalBounds();
    auto envBounds = fBounds.removeFromBottom(fBounds.getHeight() / 2);
    envComponent.setBounds(envBounds);
    auto dX = fBounds.getWidth() / 25;
    auto dY = fBounds.getHeight() / 16;
    ratioSlider.setBounds(dX, 4 * dX, 4.5 * dX, 4.5 * dX);
    modSlider.setBounds(6 * dX, 4 * dX, 4.5 * dX, 4.5 * dX);
    levelSlider.setBounds(11 * dX, 4 * dX, 4.5 * dX, 4.5 * dX);
    panSlider.setBounds(16 * dX, 4 * dX, 4.5 * dX, 4.5 * dX);
    
    ratioName.placeRelative(ratioSlider, 4, 12, true);
    modName.placeRelative(modSlider, 5, 10, true);
    panName.placeRelative(panSlider, 5, 10, true);
    levelName.placeRelative(levelSlider, 5, 10, true);
    
    outButton.setBounds(16 * dX, dX, 6 * dX, 2 * dX);
    
    ratioLabel.resized();
    modLabel.resized();
    panLabel.resized();
    levelLabel.resized();
    
    waveSelect.setBounds(dX, 10 * dY, 16 * dX, 3 * dY);
}

void OperatorComponent::paint(juce::Graphics &g)
{
    g.setColour(juce::Colours::black);
    g.fillRect(levelName.getBounds());
    g.fillRect(panName.getBounds());
    g.fillRect(ratioName.getBounds());
    g.fillRect(modName.getBounds());
}
