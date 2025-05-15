#include "FileSystem.h"

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
  }
}
