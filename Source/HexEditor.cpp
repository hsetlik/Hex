/*
  ==============================================================================

    HexEditor.cpp
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "HexEditor.h"

HexEditor::HexEditor(apvts* tree) : linkedTree(tree), modGrid(tree)
{
    addAndMakeVisible(&modGrid);
    for(int i = 0; i < NUM_OPERATORS; ++i)
    {
        addAndMakeVisible(opComponents.add(new OperatorComponent(i, linkedTree)));
    }
}

void HexEditor::resized()
{
    auto gridWidth = getWidth() / 6;
    auto bounds = getLocalBounds();
    auto rightColumn = bounds.removeFromRight(gridWidth);
    auto gridBounds = rightColumn.removeFromTop(gridWidth);
    modGrid.setBounds(gridBounds);
    
    auto dX = bounds.getWidth() / 3;
    auto topBounds = bounds.removeFromTop(bounds.getHeight() / 2);
    opComponents[0]->setBounds(topBounds.removeFromLeft(dX));
    opComponents[1]->setBounds(topBounds.removeFromLeft(dX));
    opComponents[2]->setBounds(topBounds);
    
    opComponents[3]->setBounds(bounds.removeFromLeft(dX));
    opComponents[4]->setBounds(bounds.removeFromLeft(dX));
    opComponents[5]->setBounds(bounds);
}
