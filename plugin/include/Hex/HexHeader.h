#pragma once
#include <JuceHeader.h>
#include "juce_audio_processors/juce_audio_processors.h"
typedef juce::String String;
typedef juce::Component Component;
typedef juce::File File;
typedef juce::ValueTree ValueTree;
typedef juce::Identifier Identifier;
typedef juce::ParameterAttachment ParamAttachment;
using apvts = juce::AudioProcessorValueTreeState;
using Color = juce::Colour;

#define NUM_OPERATORS 6

enum WaveType { Sine, Square, Saw, Tri, Noise };

namespace SampleRate {
void set(double rate);
double get();
}  // namespace SampleRate
