/*
  ==============================================================================

    WaveGraphComponent.cpp
    Created: 10 Jun 2021 2:37:56pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WaveGraphComponent.h"

BasicOsc::BasicOsc(WaveType type) : oscType(type)
{
    switch(oscType)
    {
        case Sine:
        {
            auto dPhase = juce::MathConstants<float>::twoPi / (float)WAVE_RES;
            for(int i = 0; i < WAVE_RES; ++i)
            {
                waveData[i] = std::sin(dPhase * (float)i);
            }
            break;
        }
        case Square:
        {
            for(int i = 0; i < WAVE_RES; ++i)
            {
                if(i < (WAVE_RES / 2))
                    waveData[i] = -1.0f;
                else
                    waveData[i] = 1.0f;
            }
            break;
        }
        case Saw:
        {
            auto dLevel = 2.0f / (float)WAVE_RES;
            for(int i = 0; i < WAVE_RES; ++i)
            {
                waveData[i] = -1.0f + dLevel * i;
            }
            break;
        }
        case Tri:
        {
            auto dLevel = 4.0f / (float)WAVE_RES;
            float level = -1.0f;
            for(int i = 0; i < WAVE_RES; ++i)
            {
                waveData[i] = level;
                if(i < WAVE_RES / 2)
                    level += dLevel;
                else
                    level -= dLevel;
            }
            break;
        }
    }
}

WaveGraph::WaveGraph(GraphParamSet* params) : linkedParams(params), fundamental(0.0f)
{
    startTimerHz(24);
}

void WaveGraph::paint(juce::Graphics &g)
{
    g.setColour(UXPalette::lightGray);
    g.fillAll();
    g.setColour(UXPalette::highlight);
    auto stroke = juce::PathStrokeType(1.5f);
    g.strokePath(trace, stroke);
}

void WaveGraph::handleAsyncUpdate()
{
    wavePoints = linkedParams->dataValues;
    updateTrace();
    repaint();
}

void WaveGraph::updateTrace()
{
    auto fBounds = getLocalBounds().toFloat();
    trace.clear();
    auto amplitude = (fBounds.getHeight() / 2.0f) * 0.8f;
    auto dX = fBounds.getWidth() / 256;
    auto y0 = fBounds.getHeight() / 2.0f;
    trace.startNewSubPath(0.0f, y0);
    for(int i = 0; i < 256; ++i)
    {
        trace.lineTo(dX * i, y0 + amplitude * wavePoints[i]);
    }
}

