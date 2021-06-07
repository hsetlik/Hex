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
    pDelay->addListener(this);
    pAttack->addListener(this);
    pHold->addListener(this);
    pDecay->addListener(this);
    pSustain->addListener(this);
    pRelease->addListener(this);
    
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
    auto radioNum = 60 + opIndex;
    bSine.setRadioGroupId(radioNum);
    bSquare.setRadioGroupId(radioNum);
    bSaw.setRadioGroupId(radioNum);
    bTri.setRadioGroupId(radioNum);
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
