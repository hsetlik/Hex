#include "FileSystem.h"
#include "Identifiers.h"
#include "juce_core/juce_core.h"

namespace UserFiles {
static const String patchFileExtension = ".hxp";
File getPatchFolder() {
  auto appData = File::getSpecialLocation(File::userApplicationDataDirectory);
  auto patchFolder = appData.getChildFile("HexPatchLibrary");
  if (!patchFolder.exists())
    patchFolder.createDirectory();
  return patchFolder;
}

ValueTree loadStateForPatch(const String& name) {
  auto fileName = name + patchFileExtension;
  auto patchFolder = getPatchFolder();
  auto patchFile = patchFolder.getChildFile(fileName);
  jassert(patchFile.existsAsFile());
  auto xmlString = patchFile.loadFileAsString();
  auto vt = ValueTree::fromXml(xmlString);
  return vt;
}

File getPatchFile(const String& patchName) {
  auto fileName = patchName + patchFileExtension;
  auto patchFolder = getPatchFolder();
  auto file = patchFolder.getChildFile(fileName);
  if (!file.existsAsFile())
    file.create();
  return file;
}

}  // namespace UserFiles

PatchLibrary::PatchLibrary() {
  auto patchFolder = UserFiles::getPatchFolder();
  auto files = patchFolder.findChildFiles(File::findFiles, true,
                                          "*" + UserFiles::patchFileExtension);
  for (auto& f : files) {
    auto xmlString = f.loadFileAsString();
    auto vt = ValueTree::fromXml(xmlString);
    jassert(vt.hasType(ID::HEX_STATE_TREE));
    auto patchTree = vt.getChildWithName(ID::HEX_PATCH_INFO);
    String name = patchTree[ID::patchName];
    String author = patchTree[ID::patchAuthor];
    int categ = patchTree[ID::patchType];
    patches.push_back({name, author, categ});
  }
}

juce::StringArray PatchLibrary::availablePatchNames() const {
  juce::StringArray names;
  for (auto& p : patches) {
    names.add(p.name);
  }
  names.sortNatural();
  return names;
}

PatchStatusE PatchLibrary::validatePatch(const patch_info_t& info) const {
  if (isNameTaken(info.name))
    return PatchStatusE::Existing;
  if (isNameLegal(info.name))
    return PatchStatusE::Available;
  return PatchStatusE::Illegal;
}

bool PatchLibrary::isNameTaken(const String& name) const {
  for (auto& p : patches) {
    if (p.name == name) {
      return true;
    }
  }
  return false;
}

bool PatchLibrary::isNameLegal(const String& name) const {
  if (name == "Untitled")
    return false;
  String rawName = name + UserFiles::patchFileExtension;
  String legalName = File::createLegalFileName(rawName);
  return rawName == legalName;
}

void PatchLibrary::loadPatch(apvts* state,
                             ValueTree& patchTree,
                             const String& name) {
  selectedPatchName = name;
  auto newTree = UserFiles::loadStateForPatch(name);
  patchTree = newTree.getChildWithName(ID::HEX_PATCH_INFO);
  newTree.removeChild(patchTree, nullptr);
  state->replaceState(newTree);
}

void PatchLibrary::savePatch(apvts* state, const patch_info_t& patch) {
  auto parent = state->copyState();
  ValueTree patchTree(ID::HEX_PATCH_INFO);
  jassert(patchTree.isValid());
  patchTree.setProperty(ID::patchName, patch.name, nullptr);
  patchTree.setProperty(ID::patchAuthor, patch.author, nullptr);
  patchTree.setProperty(ID::patchType, patch.type, nullptr);
  parent.appendChild(patchTree, nullptr);

  auto xmlString = parent.toXmlString();
  auto file = UserFiles::getPatchFile(patch.name);
  jassert(file.replaceWithText(xmlString));

  auto status = validatePatch(patch);
  selectedPatchName = patch.name;
  if (status == PatchStatusE::Available) {
    patches.push_back(patch);
    for (auto l : listeners) {
      l->newPatchSaved(patch.name);
    }
  }
  if (status == PatchStatusE::Existing) {
    for (auto l : listeners) {
      l->existingPatchSaved(patch.name);
    }
  }
}

String PatchLibrary::nameAtIndex(int idx) const {
  jassert(idx < getNumPatches());
  auto names = availablePatchNames();
  return names[idx];
}

int PatchLibrary::indexForName(const String& name) const {
  auto patchNames = availablePatchNames();
  for (int i = 0; i < patchNames.size(); ++i) {
    if (patchNames[i] == name)
      return i;
  }
  return -1;
}

patch_info_t PatchLibrary::infoForIndex(int idx) const {
  patch_info_t info;
  if (idx < 0 || idx < (int)patches.size()) {
    auto name = availablePatchNames()[idx];
    for (auto& p : patches) {
      if (p.name == name) {
        info = p;
        return info;
      }
    }
  } else {
    jassert(false);
  }
  return info;
}

int PatchLibrary::currentPatchIndex() const {
  if (selectedPatchName != "Untitled") {
    return indexForName(selectedPatchName);
  }
  return -1;
}

void PatchLibrary::addListener(Listener* l) {
  listeners.push_back(l);
}

void PatchLibrary::removeListener(Listener* l) {
  for (auto it = listeners.begin(); it != listeners.end(); ++it) {
    if (*it == l) {
      listeners.erase(it);
      return;
    }
  }
}
