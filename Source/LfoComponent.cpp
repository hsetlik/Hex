/*
  ==============================================================================

    LfoComponent.cpp
    Created: 25 Jun 2021 4:43:39pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LfoComponent.h"
DualModeSlider::DualModeSlider() : snapMode(false), bpm(120.0f)
{
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
void DualModeSlider::calculateHzValues()
{
    hzValues.clear();
    for(auto length : noteLengths)
    {
        hzValues.push_back(NoteLength::periodHz(length.first, length.second, bpm));
    }
}
double DualModeSlider::snapValue(double attemptedValue, juce::Slider::DragMode dragMode)
{
    if(!snapMode)
        return attemptedValue;
    float minValue = std::numeric_limits<float>::max();
    int idx = 0;
    for(int i = 0; i < (int)hzValues.size(); ++i)
    {
        if(std::abs(hzValues[i] - attemptedValue) < minValue)
        {
            minValue = std::abs(hzValues[i] - attemptedValue);
            idx = i;
        }
    }
    return (double)hzValues[idx];
}
std::pair<int, int> DualModeSlider::currentNoteLength()
{
    auto attemptedValue = getValue();
    float minValue = std::numeric_limits<float>::max();
    int idx = 0;
    for(int i = 0; i < (int)hzValues.size(); ++i)
    {
        if(std::abs(hzValues[i] - attemptedValue) < minValue)
        {
            minValue = std::abs(hzValues[i] - attemptedValue);
            idx = i;
        }
    }
    return noteLengths[idx];
}
bool DualModeSlider::setSync(int num, int denom)
{
    if(!snapMode)
        return false;
    int idx = 0;
    for(auto length : noteLengths)
    {
        if(std::make_pair(num, denom) == length)
        {
            setValue((double)hzValues[idx]);
            return true;
        }
    }
    return false;
}
//======================================================================================
DualModeLabel::DualModeLabel(DualModeSlider* s) : linkedSlider(s)
{
    linkedSlider->addListener(this);
    if(linkedSlider->inSnapMode())
    {
        auto note = linkedSlider->currentNoteLength();
        lastStr = juce::String(note.first) + " / " + juce::String(note.second);
    }
    else
    {
        lastStr = juce::String((float)linkedSlider->getValue());
    }
    setText(lastStr, juce::dontSendNotification);
}
void DualModeLabel::sliderValueChanged(juce::Slider *s)
{
    if(linkedSlider->inSnapMode())
    {
        auto note = linkedSlider->currentNoteLength();
        setTextNoteLength(note.first, note.second);
    }
    else
    {
        setTextHz((float)linkedSlider->getValue());
    }
}
void DualModeLabel::setTextHz(float value)
{
    setText(juce::String(value), juce::dontSendNotification);
}
void DualModeLabel::setTextNoteLength(int num, int denom)
{
    auto str = juce::String(num) + " / " + juce::String(denom);
    setText(str, juce::dontSendNotification);
}
void DualModeLabel::textWasEdited()
{
    auto str = getText();
    if(linkedSlider->inSnapMode() && str.contains("/"))
    {
        auto frac = stdu::stringAsFraction(str.toStdString());
        if(linkedSlider->setSync(frac.first, frac.second))
            setTextNoteLength(frac.first, frac.second);
        else
            setText(lastStr, juce::dontSendNotification);
    }
    else
    {
        float value = std::stof(str.toStdString());
        if(value >= linkedSlider->getMinimum() && value <= linkedSlider->getMaximum())
        {
            linkedSlider->setValue(value);
        }
    }
    lastStr = str;
}

//======================================================================================

LfoComponent::LfoComponent(int i, juce::AudioProcessor* proc, GraphParamSet* gParams, apvts* tree) :
lfoIndex(i),
linkedProcessor(proc),
linkedParams(gParams),
linkedTree(tree),
rateLabel(&rateSlider),
waveSelect(i, tree, "lfoWaveParam")
{
    auto iStr = juce::String(lfoIndex);
    auto rateId = "lfoRateParam" + iStr;
    rateAttach.reset(new sliderAttach(*linkedTree, rateId, rateSlider));
    auto syncId = "lfoSyncParam" + iStr;
    syncAttach.reset(new buttonAttach(*linkedTree, syncId, bpmToggle));
    
    addAndMakeVisible(&bpmToggle);
    bpmToggle.setButtonText("Sync");
    bpmToggle.setClickingTogglesState(true);
    bpmToggle.addListener(this);
    
    addAndMakeVisible(&rateSlider);
    rateSlider.setSliderStyle(juce::Slider::Rotary);
    rateSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
    addAndMakeVisible(&rateLabel);
    
    addAndMakeVisible(&waveSelect);
    
    startTimerHz(2);
    prepare();
}

void LfoComponent::buttonClicked(juce::Button *b)
{
    rateSlider.toggleSnapMode();
    rateLabel.textWasEdited();
}

void LfoComponent::prepare()
{
    linkedProcessor->getPlayHead()->getCurrentPosition(currentPos);
    bpm = (float)currentPos.bpm;
}

void LfoComponent::resized()
{
    auto bounds = getLocalBounds();
    auto sWidth = bounds.getWidth() / 3;
    auto lBounds = bounds.removeFromLeft(sWidth);
    auto dY = lBounds.getHeight() / 2;
    auto cushion = lBounds.getWidth() / 8;
    auto sliderBounds = lBounds.removeFromBottom(dY).reduced(cushion);
    sliderBounds = sliderBounds.withY(sliderBounds.getY() * 0.65f);
    rateSlider.setBounds(sliderBounds);
    rateLabel.setBounds(ComponentUtil::boxBelow(rateSlider, 7));
    cushion = lBounds.getWidth() / 6;
    bpmToggle.setBounds(lBounds.reduced(cushion, 2.5f * cushion));
    
    auto waveBounds = bounds;
    waveBounds = waveBounds.removeFromRight(bounds.getWidth() * 0.75f);
    dY = waveBounds.getHeight() / 4;
    waveBounds = waveBounds.removeFromTop(dY);
    waveBounds = waveBounds.withY(waveBounds.getY() + waveBounds.getHeight() * 0.5f);
    waveSelect.setBounds(waveBounds);
}

void LfoComponent::timerCallback() {prepare(); }

