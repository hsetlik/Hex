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
    VoiceClickTest(int voiceIdx, float sampleA, float sampleB) :
    voice(voiceIdx),
    s1(sampleA),
    s2(sampleB)
    {
    }
    void printTest() override
    {
        printf("ON VOICE %d:\n", voice);
        printf("jumped from %f to %f\n", s1, s2);
    }
private:
    int voice;
    float s1, s2;
};

class DebugSingle
{
public:
    static DebugSingle* getInstance();
    void addClickTest(int voice, float sA, float sB)
    {
        tests.add(new VoiceClickTest(voice, sA, sB));
    }
    ~DebugSingle();
private:
    DebugSingle()
    {
        
    }
    juce::OwnedArray<DebugTest> tests;
    static std::unique_ptr<DebugSingle> instance;
};
