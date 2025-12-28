#pragma once
#include "Audio/Synthesizer.h"
#include "GUI/SliderLabel.h"
#include "ComponentUtil.h"
#include "HexHeader.h"
#include "Color.h"

#define REPAINT_FPS 24
#define NOISE_SEED 2239

class EnvelopeComponent : public juce::Component {
public:
  class DAHDSRGraph : public juce::Component,
                      public juce::Slider::Listener,
                      public juce::Timer {
  public:
    DAHDSRGraph(EnvelopeComponent* parent);
    void sliderValueChanged(juce::Slider* slider) override;
    void paint(juce::Graphics& g) override;
    void timerCallback() override;
    juce::Slider* const pDelay;
    juce::Slider* const pAttack;
    juce::Slider* const pHold;
    juce::Slider* const pDecay;
    juce::Slider* const pSustain;
    juce::Slider* const pRelease;
    float delayVal;
    float attackVal;
    float holdVal;
    float decayVal;
    float sustainVal;
    float releaseVal;
    bool needsRepaint;
  };
  class LevelMeter : public juce::Component,
                     public juce::AsyncUpdater,
                     public juce::Timer {
  public:
    LevelMeter(int idx, GraphParamSet* params, bool filter);
    const int envIndex;
    const bool isFilter;
    GraphParamSet* const linkedParams;
    void timerCallback() override;
    void handleAsyncUpdate() override;
    void paint(juce::Graphics& g) override;

  private:
    float level;
    int lastVoice;
  };
  EnvelopeComponent(int idx,
                    apvts* tree,
                    GraphParamSet* gParams,
                    bool isFilterComp = false);
  ~EnvelopeComponent() override;
  const int opIndex;
  const bool isFilter;
  apvts* const linkedTree;
  void resized() override;

  DAHDSRGraph graph;

  LevelMeter meter;

  juce::Slider delaySlider;
  juce::Slider attackSlider;
  juce::Slider holdSlider;
  juce::Slider decaySlider;
  juce::Slider sustainSlider;
  juce::Slider releaseSlider;

  VerticalParamName delayName;
  VerticalParamName attackName;
  VerticalParamName holdName;
  VerticalParamName decayName;
  VerticalParamName sustainName;
  VerticalParamName releaseName;

  pSliderAttach delayAttach;
  pSliderAttach attackAttach;
  pSliderAttach holdAttach;
  pSliderAttach decayAttach;
  pSliderAttach sustainAttach;
  pSliderAttach releaseAttach;
};

