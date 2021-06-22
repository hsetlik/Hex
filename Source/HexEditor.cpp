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
    
    typeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(*linkedTree, "filterTypeParam", typeBox));
    typeBox.addItem("Low Pass", 1);
    typeBox.addItem("High Pass", 2);
    typeBox.addItem("Band Pass", 3);
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
    
}

void FilterPanel::resized()
{
    auto bounds = getLocalBounds();
    auto sWidth = bounds.getWidth() / 4;
    auto upperBounds = bounds.removeFromTop(sWidth);
    auto midBounds = bounds.removeFromTop(sWidth);
    auto cushion = midBounds.getHeight() / 5;
    typeBox.setBounds(midBounds.reduced(cushion));
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
HexEditor::HexEditor(HexAudioProcessor* proc, apvts* tree, GraphParamSet* params, RingBuffer<GLfloat>* buffer) :
linkedTree(tree),
modGrid(tree),
graph(params, buffer),
fPanel(tree, params),
loader(proc, &saveDialog),
saveDialog(&loader)
{
    addAndMakeVisible(&modGrid);
    addAndMakeVisible(&graph);
    addAndMakeVisible(&fPanel);
    addAndMakeVisible(&loader);
    addAndMakeVisible(&saveDialog);
    saveDialog.setEnabled(false);
    saveDialog.setVisible(false);
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
    auto loaderHeight = rightColumn.getWidth() / 3;
    auto loaderBounds = rightColumn.removeFromTop(loaderHeight);
    loader.setBounds(loaderBounds);
    modGrid.setBounds(rightColumn.removeFromTop(gridWidth));
    auto gBounds = rightColumn.removeFromTop(gridWidth);
    auto cushion = gBounds.getWidth() / 15;
    graph.setBounds(gBounds.reduced(cushion));
    fPanel.setBounds(rightColumn);
    
    auto dX = bounds.getWidth() / 3;
    auto topBounds = bounds.removeFromTop(bounds.getHeight() / 2);
    opComponents[0]->setBounds(topBounds.removeFromLeft(dX));
    opComponents[1]->setBounds(topBounds.removeFromLeft(dX));
    opComponents[2]->setBounds(topBounds);
    
    opComponents[3]->setBounds(bounds.removeFromLeft(dX));
    opComponents[4]->setBounds(bounds.removeFromLeft(dX));
    opComponents[5]->setBounds(bounds);
    
    auto xCushion = getWidth() / 3;
    auto yCushion = getHeight() / 3;
    auto saveBounds = getLocalBounds().reduced(xCushion, yCushion);
    saveDialog.setBounds(saveBounds);
}

void HexEditor::paint(juce::Graphics &g)
{
    g.setColour(UXPalette::lightGray);
    g.fillAll();
}
