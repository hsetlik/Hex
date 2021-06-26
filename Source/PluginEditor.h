/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include "HexEditor.h"
#include <JuceHeader.h>
#include "PatchBrowser.h"

//==============================================================================
/**
*/
class HexAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HexAudioProcessorEditor (HexAudioProcessor&);
    ~HexAudioProcessorEditor() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HexAudioProcessor& audioProcessor;
    
    HexEditor mainEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HexAudioProcessorEditor)
};
