
#include "PatchBrowser.h"
#include "Color.h"
#include "FileSystem.h"
#include "Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

//========================================

PatchComboBox::PatchComboBox(HexState* s) : state(s) {
  // 1. combo box
  cb.setTextWhenNoChoicesAvailable("Untitled");
  cb.addItemList(s->patchLib.availablePatchNames(), 1);
  String currentName = state->patchTree[ID::patchName];
  if (currentName != "Untitled") {
    auto idx = state->patchLib.indexForName(currentName);
    jassert(idx != -1);
    cb.setSelectedItemIndex(idx);
  }
  addAndMakeVisible(cb);
  cb.addListener(this);

  // 2. buttons
  addAndMakeVisible(lButton);
  lButton.onClick = [this]() {
    auto idx = cb.getSelectedItemIndex() - 1;
    cb.setSelectedItemIndex(idx);
  };

  addAndMakeVisible(rButton);
  rButton.onClick = [this]() {
    auto idx = cb.getSelectedItemIndex() + 1;
    cb.setSelectedItemIndex(idx);
  };
  updateButtonEnablement();
  // 3. attach patchlib listener
  state->patchLib.addListener(this);
}

PatchComboBox::~PatchComboBox() {
  cb.removeListener(this);
  state->patchLib.removeListener(this);
}

void PatchComboBox::updateButtonEnablement() {
  const int idx = cb.getSelectedItemIndex();
  lButton.setEnabled(idx > 0);
  rButton.setEnabled(idx < cb.getNumItems() - 1);
}

void PatchComboBox::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float dX = fBounds.getHeight();
  static const float inset = 2.0f;
  auto lBounds = fBounds.removeFromLeft(dX).reduced(inset);
  lButton.setBounds(lBounds.toNearestInt());
  auto rBounds = fBounds.removeFromLeft(dX).reduced(inset);
  rButton.setBounds(rBounds.toNearestInt());
  auto boxBounds = fBounds.reduced(inset);
  cb.setBounds(boxBounds.toNearestInt());
}

void PatchComboBox::comboBoxChanged(juce::ComboBox* box) {
  auto name = box->getText();
  state->patchLib.loadPatch(&state->mainTree, state->patchTree, name);
  updateButtonEnablement();
}

void PatchComboBox::existingPatchSaved(const String& name) {
  auto idx = state->patchLib.indexForName(name);
  jassert(idx > -1);
  cb.setSelectedItemIndex(idx, juce::dontSendNotification);
  updateButtonEnablement();
}

void PatchComboBox::newPatchSaved(const String& name) {
  auto newIndex = state->patchLib.indexForName(name);
  cb.addItem(name, newIndex + 1);
  cb.setSelectedItemIndex(newIndex, juce::dontSendNotification);
  updateButtonEnablement();
}

void PatchComboBox::existingPatchLoaded(const String& name) {
  auto idx = state->patchLib.indexForName(name);
  jassert(idx > -1);
  cb.setSelectedItemIndex(idx, juce::dontSendNotification);
  updateButtonEnablement();
}
//========================================

PatchLoader::PatchLoader(HexState* s)
    : cb(s), saveBtn("Save Patch"), loadBtn("Load Patch") {
  addAndMakeVisible(&cb);
  addAndMakeVisible(&saveBtn);
  addAndMakeVisible(&loadBtn);

  // set up onClick lambdas
  saveBtn.onClick = [this, s]() {
    auto* parent = getBrowserParent();
    String patchName = s->patchTree[ID::patchName];
    parent->openSaveDialog(patchName);
  };
  loadBtn.onClick = [this, s]() {
    auto* parent = getBrowserParent();
    String patchName = s->patchTree[ID::patchName];
    parent->openSaveDialog(patchName);
  };
}

PatchBrowserParent* PatchLoader::getBrowserParent() {
  auto* comp = findParentComponentOfClass<PatchBrowserParent>();
  jassert(comp != nullptr);
  return comp;
}

void PatchLoader::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float cbHeight = 45.0f;
  auto cbBox = fBounds.removeFromTop(cbHeight).reduced(2.0f).toNearestInt();
  cb.setBounds(cbBox);
  const float btnWidth = fBounds.getWidth() / 2.0f;
  saveBtn.setBounds(
      fBounds.removeFromLeft(btnWidth).reduced(2.0f).toNearestInt());

  loadBtn.setBounds(fBounds.reduced(2.0f).toNearestInt());
}

//========================================

SaveDialog::SaveDialog(HexState* s) : state(s) {
  // labels
  nameLabel.setText("Patch Name:", juce::dontSendNotification);
  nameLabel.setEditable(false);
  addAndMakeVisible(nameLabel);
  authorLabel.setText("Author:", juce::dontSendNotification);
  authorLabel.setEditable(false);
  addAndMakeVisible(authorLabel);
  typeLabel.setText("Category:", juce::dontSendNotification);
  typeLabel.setEditable(false);
  addAndMakeVisible(typeLabel);

  // editors
  nameEditor.setMultiLine(false);
  nameEditor.setReturnKeyStartsNewLine(false);
  nameEditor.setEscapeAndReturnKeysConsumed(false);
  addAndMakeVisible(nameEditor);
  nameEditor.addListener(this);

  authEditor.setMultiLine(false);
  authEditor.setReturnKeyStartsNewLine(false);
  authEditor.setEscapeAndReturnKeysConsumed(false);
  addAndMakeVisible(authEditor);
  authEditor.addListener(this);

  // type combo box
  typeBox.addItemList(patchTypeNames, 1);
  typeBox.setSelectedItemIndex(0);
  addAndMakeVisible(typeBox);

  // save and close buttons
  nSaveButton.setButtonText("Save New");
  nSaveButton.setEnabled(false);
  nSaveButton.onClick = [this]() { saveAndClose(); };
  addAndMakeVisible(nSaveButton);

  eSaveButton.setButtonText("Save Existing");
  eSaveButton.setEnabled(false);
  eSaveButton.onClick = [this]() { saveAndClose(); };
  addAndMakeVisible(eSaveButton);

  cancelButton.setButtonText("Cancel");
  cancelButton.onClick = [this]() {
    auto* parent = findParentComponentOfClass<PatchBrowserParent>();
    if (parent != nullptr)
      parent->closeModal();
  };
  addAndMakeVisible(cancelButton);
}

SaveDialog::~SaveDialog() {
  nameEditor.removeListener(this);
  authEditor.removeListener(this);
}

void SaveDialog::textEditorTextChanged(juce::TextEditor& ed) {
  juce::ignoreUnused(ed);
  auto info = getCurrentInfo();
  auto val = state->patchLib.validatePatch(info);
  switch (val) {
    case PatchStatusE::Available:
      nSaveButton.setEnabled(true);
      eSaveButton.setEnabled(false);
      break;
    case PatchStatusE::Existing:
      nSaveButton.setEnabled(false);
      eSaveButton.setEnabled(true);
      break;
    case PatchStatusE::Illegal:
      nSaveButton.setEnabled(false);
      eSaveButton.setEnabled(false);
      break;
  }
}

patch_info_t SaveDialog::getCurrentInfo() const {
  patch_info_t info;
  info.name = nameEditor.getText();
  info.author = authEditor.getText();
  info.type = typeBox.getSelectedItemIndex();
  return info;
}

bool SaveDialog::isPatchLegal() const {
  auto info = getCurrentInfo();
  return state->patchLib.validatePatch(info) != PatchStatusE::Illegal;
}

void SaveDialog::saveAndClose() {
  auto info = getCurrentInfo();
  // save the existing patch
  state->patchLib.savePatch(&state->mainTree, info);
  // close the modal window
  auto* parent = findParentComponentOfClass<PatchBrowserParent>();
  if (parent != nullptr) {
    parent->closeModal();
  }
}

void SaveDialog::initializeFor(const String& patchName) {
  auto patchIdx = state->patchLib.indexForName(patchName);
  // if a patch of this name already exists
  if (patchIdx != -1) {
    auto info = state->patchLib.infoForIndex(patchIdx);
    nameEditor.setText(info.name);
    authEditor.setText(info.author);
    typeBox.setSelectedItemIndex(info.type);
    eSaveButton.setEnabled(true);
    nSaveButton.setEnabled(false);
  } else {
    nameEditor.setText(patchName);
    authEditor.setText("User");
    typeBox.setSelectedItemIndex(0);
  }
}

void SaveDialog::resized() {
  auto fBounds = getLocalBounds().toFloat().reduced(3.5f);
  const float dY = juce::jmax(fBounds.getHeight() / 7.0f, 24.0f);
  const float dX = juce::jmin(fBounds.getWidth(), 300.0f);
  const float border = 1.5f;
  // place things top to bottom
  auto nlBounds = fBounds.removeFromTop(dY).reduced(border);
  nameLabel.setBounds(nlBounds.toNearestInt());
  auto neBounds = fBounds.removeFromTop(dY).reduced(border);
  nameEditor.setBounds(neBounds.toNearestInt());
  auto alBounds = fBounds.removeFromTop(dY).reduced(border);
  authorLabel.setBounds(alBounds.toNearestInt());
  auto aeBounds = fBounds.removeFromTop(dY).reduced(border);
  authEditor.setBounds(aeBounds.toNearestInt());
  auto tlBounds = fBounds.removeFromTop(dY).reduced(border);
  typeLabel.setBounds(tlBounds.toNearestInt());
  auto tbBounds = fBounds.removeFromTop(dY).reduced(border);
  typeBox.setBounds(tbBounds.toNearestInt());
  // place the buttons side to side
  auto buttonBounds = fBounds.removeFromTop(dY);
  buttonBounds = buttonBounds.withSizeKeepingCentre(
      buttonBounds.getWidth() * 0.75f, buttonBounds.getHeight());
  const float buttonWidth = buttonBounds.getWidth() / 3.0f;
  auto cbBounds = buttonBounds.removeFromLeft(buttonWidth).reduced(border);
  cancelButton.setBounds(cbBounds.toNearestInt());
  auto nsbBounds = buttonBounds.removeFromLeft(buttonWidth).reduced(border);
  nSaveButton.setBounds(nsbBounds.toNearestInt());
  auto esbBounds = buttonBounds.reduced(border);
  eSaveButton.setBounds(esbBounds.toNearestInt());
}

void SaveDialog::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UXPalette::highlight);
  g.fillRect(fBounds);
  g.setColour(UXPalette::darkBkgnd);
  g.fillRect(fBounds.reduced(3.5f));
}
