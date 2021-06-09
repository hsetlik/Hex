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
EnvelopeComponent::EnvelopeComponent(int idx, apvts* tree) :
opIndex(idx),
linkedTree(tree),
graph(this)
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
    
    auto iStr = juce::String(opIndex);
    auto delayId = "delayParam" + iStr;
    auto attackId = "attackParam" + iStr;
    auto holdId = "holdParam" + iStr;
    auto decayId = "decayParam" + iStr;
    auto sustainId = "sustainParam" + iStr;
    auto releaseId = "releaseParam" + iStr;
    
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
    graph.setBounds(bounds.removeFromTop(dY));
    auto dX = bounds.getWidth() / 6;
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
//=======================================================
WaveSelector::WaveSelector(int index, apvts* tree) :
opIndex(index),
linkedTree(tree)
{
    addAndMakeVisible(&bSine);
    addAndMakeVisible(&bSquare);
    addAndMakeVisible(&bSaw);
    addAndMakeVisible(&bTri);
    addAndMakeVisible(hiddenBox);
    hiddenBox.setVisible(false);
    hiddenBox.addItem("Sine", 1);
    hiddenBox.addItem("Square", 2);
    hiddenBox.addItem("Saw", 3);
    hiddenBox.addItem("Tri", 4);
    auto waveId = "waveParam" + juce::String(opIndex);
    hiddenBoxAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(*linkedTree, waveId, hiddenBox));
    auto radioNum = 60 + opIndex;
    bSine.setRadioGroupId(radioNum);
    bSquare.setRadioGroupId(radioNum);
    bSaw.setRadioGroupId(radioNum);
    bTri.setRadioGroupId(radioNum);
    
    bSine.triggerClick();
    
    bSine.addListener(this);
    bSquare.addListener(this);
    bSaw.addListener(this);
    bTri.addListener(this);
}

void WaveSelector::resized()
{
    auto fBounds = getLocalBounds();
    auto dX = fBounds.getWidth() / 4;
    bSine.setBounds(fBounds.removeFromLeft(dX));
    bSquare.setBounds(fBounds.removeFromLeft(dX));
    bSaw.setBounds(fBounds.removeFromLeft(dX));
    bTri.setBounds(fBounds);
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
    auto pString = "waveParam" + juce::String(opIndex);
    auto val = linkedTree->getRawParameterValue(pString)->load();
    printf("Oscillator %d wave param: %f\n", opIndex, val);
}

//=======================================================
OperatorComponent::OperatorComponent(int idx, apvts* tree) :
opIndex(idx),
linkedTree(tree),
envComponent(idx, tree),
waveSelect(idx, tree),
ratioLabel(&ratioSlider),
modLabel(&modSlider),
panLabel(&panSlider)
{
    addAndMakeVisible(&envComponent);
    addAndMakeVisible(&waveSelect);
    
    SliderUtil::setRotaryNoBox(ratioSlider);
    SliderUtil::setRotaryNoBox(modSlider);
    SliderUtil::setRotaryNoBox(panSlider);
    
    addAndMakeVisible(&ratioSlider);
    addAndMakeVisible(&modSlider);
    addAndMakeVisible(&panSlider);
    addAndMakeVisible(&outButton);
    
    addAndMakeVisible(&ratioLabel);
    addAndMakeVisible(&modLabel);
    addAndMakeVisible(&panLabel);
    
    auto iStr = juce::String(opIndex);
    auto ratioId = "ratioParam" + iStr;
    auto indexId = "indexParam" + iStr;
    auto panId = "panParam" + iStr;
    auto outputId = "audibleParam" + iStr;
    
    ratioAttach.reset(new sliderAttach(*linkedTree, ratioId, ratioSlider));
    modAttach.reset(new sliderAttach(*linkedTree, indexId, modSlider));
    panAttach.reset(new sliderAttach(*linkedTree, panId, panSlider));
    outAttach.reset(new buttonAttach(*linkedTree, outputId, outButton));
    
    outButton.addListener(this);
}

void OperatorComponent::buttonClicked(juce::Button *b)
{
    if(b->getToggleState())
        panSlider.setVisible(true);
    else
        panSlider.setVisible(false);
}

void OperatorComponent::resized()
{
    auto fBounds = getLocalBounds();
    auto envBounds = fBounds.removeFromBottom(fBounds.getHeight() / 2);
    envComponent.setBounds(envBounds);
    auto dX = fBounds.getWidth() / 25;
    auto dY = fBounds.getHeight() / 16;
    ratioSlider.setBounds(dX, dX, 4.5 * dX, 4.5 * dX);
    modSlider.setBounds(6 * dX, dX, 4.5 * dX, 4.5 * dX);
    panSlider.setBounds(11 * dX, dX, 4.5 * dX, 4.5 * dX);
    outButton.setBounds(16 * dX, 2 * dX, 6 * dX, 2 * dX);
    
    ratioLabel.resized();
    modLabel.resized();
    panLabel.resized();
    
    waveSelect.setBounds(dX, 8 * dY, 16 * dX, 3 * dY);
}
