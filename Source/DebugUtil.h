/*
  ==============================================================================

    DebugUtil.h
    Created: 13 Jun 2021 3:20:34pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct DebugTest
{
    DebugTest() {}
    virtual ~DebugTest() {}
    virtual void printTest() {}
};

struct VoiceClickTest : public DebugTest
{
    VoiceClickTest (int voiceIdx, float sampleA, float sampleB) :
    voice (voiceIdx),
    s1 (sampleA),
    s2 (sampleB)
    {
    }
    void printTest() override
    {
        printf ("ON VOICE %d:\n", voice);
        printf ("jumped from %f to %f\n", s1, s2);
    }
private:
    int voice;
    float s1, s2;
};

class DebugSingle
{
public:
    static DebugSingle* getInstance();
    void addClickTest (int voice, float sA, float sB)
    {
        tests.add (new VoiceClickTest (voice, sA, sB));
    }
    ~DebugSingle();
private:
    DebugSingle()
    {
        
    }
    juce::OwnedArray<DebugTest> tests;
    static std::unique_ptr<DebugSingle> instance;
};

class AsyncDebugPrinter : public juce::AsyncUpdater
{
public:
    void addMessage (juce::String message)
    {
        messages.push_back (message);
        if(!isUpdatePending())
            triggerAsyncUpdate();
    }
    void handleAsyncUpdate() override
    {
        for (auto m : messages)
            printf("%s\n", m.toRawUTF8());
        messages.clear();
    }
    static juce::String bStr (bool input)
    {
        if (input)
            return "true";
        return "false";
    }
private:
    std::vector<juce::String> messages;
};

class BufferChecker
{
public:
    BufferChecker() {}
    ~BufferChecker() {}
    void checkBuffer (juce::AudioBuffer<float>& buffer);
private:
    void addToLog (juce::AudioBuffer<float>& buffer);
    AsyncDebugPrinter printer;
};
