/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Identifiers.h"
#include "PluginEditor.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"

//==============================================================================
HexAudioProcessor::HexAudioProcessor()
    // #ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      // #endif
      tree(this),
      synth(&tree.mainTree),
      createdEditor(nullptr) {
}

HexAudioProcessor::~HexAudioProcessor() {}

//==============================================================================
const juce::String HexAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool HexAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool HexAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool HexAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double HexAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int HexAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int HexAudioProcessor::getCurrentProgram() {
  return 0;
}

void HexAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String HexAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void HexAudioProcessor::changeProgramName(int index,
                                          const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

//==============================================================================
void HexAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  SampleRate::set(sampleRate);
  synth.setSampleRate(sampleRate, samplesPerBlock);
  synth.prepareRingBuffer(samplesPerBlock);
  // synth.prepareRingBuffer (samplesPerBlock);
}

void HexAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HexAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void HexAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals nd;
  masterKbdState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(),
                                       true);
  buffer.clear();
  synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
  synth.updateRoutingForBlock();
  synth.updateOscillatorsForBlock();
  synth.updateEnvelopesForBlock();
  synth.updateFiltersForBlock();
  synth.updateLfosForBlock();
}

//==============================================================================
bool HexAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HexAudioProcessor::createEditor() {
  createdEditor = new HexAudioProcessorEditor(*this);
  return createdEditor;
}

//==============================================================================
void HexAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = tree.mainTree.copyState();
  state.appendChild(tree.patchTree.createCopy(), nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void HexAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(tree.mainTree.state.getType())) {
      auto parentTree = juce::ValueTree::fromXml(*xmlState);
      auto childTree = parentTree.getChildWithName(ID::HEX_PATCH_INFO);
      tree.patchTree.copyPropertiesFrom(childTree, nullptr);
      parentTree.removeChild(childTree, nullptr);
      tree.mainTree.replaceState(parentTree);
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new HexAudioProcessor();
}
