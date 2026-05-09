/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HexAudioProcessorEditor::HexAudioProcessorEditor(HexAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      mainEditor(&audioProcessor,
                 &p.tree,
                 &p.synth.graphParams,
                 &p.synth.graphBuffer,
                 p.masterKbdState),
      tWindow(this) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(&mainEditor);
  addAndMakeVisible(&tWindow);
  int maxWidth = 2000;
  int maxHeight = 1800;
  int minWidth = 800;
  int minHeight = 600;
  auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
  if(display != nullptr){
    auto uArea = display->userArea;
    float fWidth = uArea.getWidth();
    float fHeight = uArea.getHeight();
    maxWidth = (int)(fWidth * 0.95f);
    maxHeight = (int)(fHeight * 0.95f);
    minWidth = (int)(fWidth * 0.45f);
    minHeight = (int)(fHeight * 0.45f);
  }
  setResizable(true, true);
  setResizeLimits(minWidth, minHeight, maxWidth, maxHeight);
  setSize(maxWidth, maxHeight);
}

HexAudioProcessorEditor::~HexAudioProcessorEditor() {}

//==============================================================================
void HexAudioProcessorEditor::paint(juce::Graphics&) {}

void HexAudioProcessorEditor::resized() {
  mainEditor.setBounds(0, 0, getWidth(), getHeight());
  tWindow.setBounds(0, 0, getWidth(), getHeight());
}
