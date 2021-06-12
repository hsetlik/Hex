/*
  ==============================================================================

    WaveGraphComponent.cpp
    Created: 10 Jun 2021 2:37:56pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WaveGraphComponent.h"

WaveGraph::WaveGraph(GraphParamSet* params, RingBuffer<GLfloat>* rBuffer) :
linkedParams(params),
ringBuffer(rBuffer),
fundamental(0.0f)
{
    
}

WaveGraph::~WaveGraph()
{
    
}

void WaveGraph::paint(juce::Graphics &g)
{
    
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

void WaveGraph::newOpenGLContextCreated()
{
    
}

void WaveGraph::openGLContextClosing()
{
    
}

void WaveGraph::renderOpenGL()
{
    
}

