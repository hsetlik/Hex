#pragma once
#include "HexHeader.h"

namespace UserFiles {
File getPatchFolder();
File getPatchFile(const String& name);
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
  std::vector<patch_info_t> patches;

public:
  PatchLibrary();
  juce::StringArray availablePatchNames() const;
  int getNumPatches() const { return (int)patches.size(); }
  PatchStatusE validatePatch(const patch_info_t& info) const;
  // actual saving/loading work happens here
  void savePatch(apvts* state, const patch_info_t& patch);
  //----------------------------
  struct Listener {
    Listener() = default;
    virtual ~Listener() {}
    virtual void newPatchSaved(const String& patchName);
    virtual void existingPatchSaved(const String& patchName);
  };
  void addListener(Listener* l);
  void removeListener(Listener* l);

private:
  std::vector<Listener*> listeners;
  bool isNameTaken(const String& name) const;
  bool isNameLegal(const String& name) const;
};
