#pragma once

#include "HexHeader.h"
#include "FileSystem.h"

class HexState {
public:
  apvts mainTree;
  PatchLibrary patchLib;
  HexState(juce::AudioProcessor* proc);
};
