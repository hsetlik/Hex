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
