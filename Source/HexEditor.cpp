/*
  ==============================================================================

    HexEditor.cpp
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "HexEditor.h"
FilterPanel::FilterPanel(apvts* tree, GraphParamSet* graph) :
linkedTree(tree),
envComp(0, linkedTree, graph, true)
{
    cutoffAttach.reset(new sliderAttach(*linkedTree, "cutoffParam", cutoffSlider));
    resAttach.reset(new sliderAttach(*linkedTree, "resonanceParam", resSlider));
    wetAttach.reset(new sliderAttach(*linkedTree, "wetDryParam", wetSlider));
    depthAttach.reset(new sliderAttach(*linkedTree, "depthParam", depthSlider));
    
    SliderUtil::setRotaryNoBox(cutoffSlider);
    SliderUtil::setRotaryNoBox(resSlider);
    SliderUtil::setRotaryNoBox(wetSlider);
    SliderUtil::setRotaryNoBox(depthSlider);
    
    addAndMakeVisible(&envComp);
    addAndMakeVisible(&cutoffSlider);
    addAndMakeVisible(&resSlider);
    addAndMakeVisible(&wetSlider);
    addAndMakeVisible(&depthSlider);
    
}

void FilterPanel::resized()
{
    auto bounds = getLocalBounds();
    auto sWidth = bounds.getWidth() / 4;
    auto upperBounds = bounds.removeFromTop(sWidth);
    cutoffSlider.setBounds(upperBounds.removeFromLeft(sWidth));
    resSlider.setBounds(upperBounds.removeFromLeft(sWidth));
    wetSlider.setBounds(upperBounds.removeFromLeft(sWidth));
    depthSlider.setBounds(upperBounds);
    envComp.setBounds(bounds);
}

void FilterPanel::paint(juce::Graphics &g)
{
    
}

//==============================================================================
HexEditor::HexEditor(apvts* tree, GraphParamSet* params, RingBuffer<GLfloat>* buffer) :
linkedTree(tree),
modGrid(tree),
graph(params, buffer),
fPanel(tree, params)
{
    addAndMakeVisible(&modGrid);
    addAndMakeVisible(&graph);
    addAndMakeVisible(&fPanel);
    for(int i = 0; i < NUM_OPERATORS; ++i)
    {
        addAndMakeVisible(opComponents.add(new OperatorComponent(i, linkedTree, params)));
    }
}

void HexEditor::resized()
{
    auto gridWidth = getWidth() / 4.5f;
    auto bounds = getLocalBounds();
    auto rightColumn = bounds.removeFromRight(gridWidth);
    modGrid.setBounds(rightColumn.removeFromTop(gridWidth));
    auto gBounds = rightColumn.removeFromTop(gridWidth);
    auto cushion = gBounds.getWidth() / 15;
    graph.setBounds(gBounds.reduced(cushion));
    fPanel.setBounds(rightColumn.removeFromTop(gridWidth));
    
    auto dX = bounds.getWidth() / 3;
    auto topBounds = bounds.removeFromTop(bounds.getHeight() / 2);
    opComponents[0]->setBounds(topBounds.removeFromLeft(dX));
    opComponents[1]->setBounds(topBounds.removeFromLeft(dX));
    opComponents[2]->setBounds(topBounds);
    
    opComponents[3]->setBounds(bounds.removeFromLeft(dX));
    opComponents[4]->setBounds(bounds.removeFromLeft(dX));
    opComponents[5]->setBounds(bounds);
}

void HexEditor::paint(juce::Graphics &g)
{
    g.setColour(UXPalette::lightGray);
    g.fillAll();
}
