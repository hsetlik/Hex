#pragma once
#include <JuceHeader.h>
#include "juce_audio_processors/juce_audio_processors.h"
typedef juce::String String;
typedef juce::Component Component;
typedef juce::File File;
typedef juce::ValueTree ValueTree;
typedef juce::Identifier Identifier;
typedef juce::ParameterAttachment ParamAttachment;
typedef std::unique_ptr<ParamAttachment> param_attach_ptr;
using apvts = juce::AudioProcessorValueTreeState;
using Color = juce::Colour;
using AttString = juce::AttributedString;
using sliderAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
using pSliderAttach =
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>;
using buttonAttach = juce::AudioProcessorValueTreeState::ButtonAttachment;
using pButtonAttach =
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>;
using comboBoxAttach = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using pComboBoxAttach =
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>;

#define NUM_OPERATORS 6

enum WaveType { Sine, Square, Saw, Tri, Noise };

namespace SampleRate {
void set(double rate);
double get();
}  // namespace SampleRate
