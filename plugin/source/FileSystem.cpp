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
