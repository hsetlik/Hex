#pragma once
#include "HexHeader.h"

namespace UserFiles {
File getPatchFolder();
ValueTree loadStateForPatch(const String& name);
}  // namespace UserFiles

//================================================

struct patch_info_t {
  String name;
  String author;
  int type;
};

enum PatchStatusE { Available, Existing, Illegal };

class PatchLibrary {
private:
  std::vector<String> patchNames;
  std::vector<patch_info_t> patches;

public:
  PatchLibrary();
  juce::StringArray availablePatchNames() const;
  int getNumPatches() const { return (int)patches.size(); }
  PatchStatusE validatePatch(const patch_info_t& info) const;

private:
  bool isNameTaken(const String& name) const;
  bool isNameLegal(const String& name) const;
};
