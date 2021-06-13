/*
  ==============================================================================

    ModulationGrid.cpp
    Created: 7 Jun 2021 2:24:59pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ModulationGrid.h"
ModulationGrid::ModulationGrid(apvts* tree) : linkedTree(tree)
{
    for(int src = 0; src < NUM_OPERATORS; ++src)
    {
        for(int dst = 0; dst < NUM_OPERATORS; ++dst)
        {
            addAndMakeVisible(buttons.add(new ModulationToggle(src, dst)));
            auto idx = (src * NUM_OPERATORS) + dst;
            auto str = juce::String(src) + "to" + juce::String(dst) + "Param";
            auto btn = buttons.getLast();
            attachments[idx].reset(new buttonAttach(*linkedTree, str, *btn));
        }
    }
}

void ModulationGrid::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto innerBounds = fBounds.reduced(fBounds.getWidth() / 8.0f);
    auto x0 = innerBounds.getX();
    auto y0 = innerBounds.getY();
    auto dX = innerBounds.getWidth() / NUM_OPERATORS;
    auto dY = innerBounds.getWidth() / NUM_OPERATORS;
    for(int src = 0; src < NUM_OPERATORS; ++src)
    {
        for(int dst = 0; dst < NUM_OPERATORS; ++dst)
        {
            auto bounds = juce::Rectangle<int>(x0 + src * dX, y0 + dst * dY, dX, dY);
            auto idx = (src * NUM_OPERATORS) + dst;
            buttons[idx]->setBounds(bounds);
        }
    }
}

void ModulationGrid::paint(juce::Graphics &g)
{
    
}
