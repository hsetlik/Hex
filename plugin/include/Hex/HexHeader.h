#pragma once
#include <JuceHeader.h>
typedef juce::String String;
typedef juce::Component Component;
typedef juce::File File;
typedef juce::ValueTree ValueTree;
using apvts = juce::AudioProcessorValueTreeState;

#define NUM_OPERATORS 6

enum WaveType { Sine, Square, Saw, Tri, Noise };

namespace SampleRate {
void set(double rate);
double get();
}  // namespace SampleRate
