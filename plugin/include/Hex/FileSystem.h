#pragma once
#include "HexHeader.h"

namespace UserFiles {
File getPatchFolder();
ValueTree loadStateForPatch(const String& name);
}  // namespace UserFiles

//================================================

class PatchLibrary {
private:
  std::vector<String> patchNames;

public:
  PatchLibrary();
  juce::StringArray availablePatchNames() const;
  int getNumPatches() const { return (int)patchNames.size(); }
};
