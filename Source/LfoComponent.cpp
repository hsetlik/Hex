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
    printf("BPM is: %f\n", bpm);
    for(auto length : noteLengths)
    {
        auto value = NoteLength::frequencyHz(length.first, length.second, bpm);
        hzValues.push_back(value);
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
depthLabel(&depthSlider),
waveSelect(i, tree, "lfoWaveParam")
{
    auto iStr = juce::String(lfoIndex);
    auto rateId = "lfoRateParam" + iStr;
    rateAttach.reset(new sliderAttach(*linkedTree, rateId, rateSlider));
    auto syncId = "lfoSyncParam" + iStr;
    syncAttach.reset(new buttonAttach(*linkedTree, syncId, bpmToggle));
    auto targetId = "lfoTargetParam" + iStr;
    targetAttach.reset(new comboBoxAttach(*linkedTree, targetId, targetBox));
    auto depthId = "lfoDepthParam" + iStr;
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
    
    addAndMakeVisible(&depthLabel);
    addAndMakeVisible(&waveSelect);
    
    startTimerHz(2);
    
    rateSlider.setLookAndFeel(&lnf);
    rateLabel.setLookAndFeel(&lnf);
    
    depthSlider.setLookAndFeel(&lnf);
    depthLabel.setLookAndFeel(&lnf);
    
    targetBox.setLookAndFeel(&lnf);
    
    prepare();
}

LfoComponent::~LfoComponent()
{
    rateSlider.setLookAndFeel(nullptr);
    rateLabel.setLookAndFeel(nullptr);
    
    depthSlider.setLookAndFeel(nullptr);
    depthLabel.setLookAndFeel(nullptr);
    
    targetBox.setLookAndFeel(nullptr);
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
    auto dX = getWidth() / 16;
    rateSlider.setBounds(dX, dX, 5 * dX, 5 * dX);
    ComponentUtil::cushionByFraction(rateSlider, 8, 8);
    depthSlider.setBounds(dX, 7 * dX, 5 * dX, 5 * dX);
    ComponentUtil::cushionByFraction(depthSlider, 8, 8);
    depthLabel.resized();
    rateLabel.setBounds(dX, 6 * dX, 5 * dX, dX);
    bpmToggle.setBounds(7 * dX, dX, 3 * dX, 1.5f * dX);
    targetBox.setBounds(8 * dX, 4 * dX, 8 * dX, 2 * dX);
    waveSelect.setBounds(6 * dX, 7 * dX, 10 * dX, 3 * dX);
}

void LfoComponent::timerCallback() {prepare(); }

juce::StringArray LfoComponent::getTargetStrings()
{
    juce::StringArray vec;
    vec.add("No Target");
    for(int i = 0; i < NUM_OPERATORS; ++i)
    {
        auto iStr = juce::String(i + 1);
        vec.add("Operator " + iStr + " level");
    }
    vec.add("Filter Cutoff");
    return vec;
}
