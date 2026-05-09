#pragma once
#include "PatchBrowser.h"

class UpperBar : public Component {
private:
  const String versionString;
  PatchLoader loader;

public:
  UpperBar(HexState* s, const String& versionStr = "Version String goes here");
  void resized() override;
  void paint(juce::Graphics& g) override;
};
