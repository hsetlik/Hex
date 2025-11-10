#pragma once

#include "HexHeader.h"
#include "FileSystem.h"

class HexState {
public:
  apvts mainTree;
  PatchLibrary patchLib;
  ValueTree patchTree;
  HexState(juce::AudioProcessor* proc);
};
