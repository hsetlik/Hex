/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HexAudioProcessorEditor::HexAudioProcessorEditor (HexAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), editor(&p.tree)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(&editor);
    setSize(1000, 800);
}

HexAudioProcessorEditor::~HexAudioProcessorEditor()
{
}

//==============================================================================
void HexAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void HexAudioProcessorEditor::resized()
{
    editor.setBounds(0, 0, getWidth(), getHeight());
}
