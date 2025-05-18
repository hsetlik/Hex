#pragma once

#include "HexHeader.h"
#include "FileSystem.h"

class HexState : public apvts {
public:
  PatchLibrary patchLib;
  HexState(juce::AudioProcessor* proc);
};
