
#include "PatchBrowser.h"
#include "Color.h"
#include "FileSystem.h"
#include "Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

PatchComboBox::PatchComboBox(HexState* s) : PatchLibrary::Listener(), state(s) {
  // 1. set up ComboBox and listener
  cb.setTextWhenNoChoicesAvailable("Untitled");
  cb.setTextWhenNothingSelected("Untitled");
  auto patchNames = state->patchLib.availablePatchNames();
  cb.addItemList(patchNames, 1);
  cb.addListener(this);
  addAndMakeVisible(cb);

  // 2. set up left & right buttons
  lButton.onClick = [this]() {
    int prevIndex = cb.getSelectedItemIndex();
    jassert(prevIndex > 0);
    cb.setSelectedItemIndex(prevIndex - 1);
  };

  rButton.onClick = [this]() {
    int prevIndex = cb.getSelectedItemIndex();
    jassert(prevIndex < cb.getNumItems() - 1);
    cb.setSelectedItemIndex(prevIndex + 1);
  };

  addAndMakeVisible(lButton);
  addAndMakeVisible(rButton);
  updateButtonEnablement();

  // 3. set up the main parameter attachment

  state->patchLib.addListener(this);
  auto* param = state->getParameter(ID::selectedPatchIndex.toString());
  auto callback = [this](float fID) { paramCallback(fID); };
  pAttach.reset(new ParamAttachment(*param, callback, nullptr));
  pAttach->sendInitialUpdate();
}

PatchComboBox::~PatchComboBox() {
  cb.removeListener(this);
  state->patchLib.removeListener(this);
}

void PatchComboBox::paramCallback(float fValue) {
  int idx = (int)fValue;
  if (idx > -1 && idx != selectedPatchIdx) {
    selectedPatchIdx = idx;

    // TODO: call the parent to load the appropriate
    // patch data here
    updateButtonEnablement();
  }
}

String PatchComboBox::getCurrentPatchName() const {
  return state->patchLib.currentPatchName();
}

void PatchComboBox::updateButtonEnablement() {
  auto currentIdx = cb.getSelectedItemIndex();
  lButton.setEnabled(currentIdx > 0);
  rButton.setEnabled(currentIdx < cb.getNumItems() - 1);
}

void PatchComboBox::comboBoxChanged(juce::ComboBox* box) {
  auto newIdx = box->getSelectedItemIndex();
  if (newIdx != selectedPatchIdx) {
    selectedPatchIdx = newIdx;
  }
}

void PatchComboBox::existingPatchSaved(const String& patchName) {
  auto idx = state->patchLib.indexForName(patchName);
  cb.setSelectedItemIndex(idx);
}

void PatchComboBox::newPatchSaved(const String& patchName) {
  auto idx = state->patchLib.indexForName(patchName);
  cb.addItem(patchName, idx + 1);
  cb.setSelectedItemIndex(idx);
}

void PatchComboBox::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float btnWidth = fBounds.getHeight();
  lButton.setBounds(
      fBounds.removeFromLeft(btnWidth).reduced(2.0f).toNearestInt());
  rButton.setBounds(
      fBounds.removeFromLeft(btnWidth).reduced(2.0f).toNearestInt());
  cb.setBounds(fBounds.toNearestInt());
}

//========================================

PatchLoader::PatchLoader(HexState* s)
    : cb(s), saveBtn("Save Patch"), loadBtn("Load Patch") {
  addAndMakeVisible(&cb);
  addAndMakeVisible(&saveBtn);
  addAndMakeVisible(&loadBtn);

  // set up onClick lambdas
  saveBtn.onClick = [this]() {
    auto* parent = getBrowserParent();
    parent->openSaveDialog(cb.getCurrentPatchName());
  };
  loadBtn.onClick = [this]() {
    auto* parent = getBrowserParent();
    parent->openLoadDialog(cb.getCurrentPatchName());
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
  saveButton.setButtonText("Save");
  saveButton.setEnabled(false);
  saveButton.onClick = [this]() { saveAndClose(); };
  addAndMakeVisible(saveButton);

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
  saveButton.setEnabled(isPatchLegal());
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
  state->patchLib.savePatch(state, info);
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
  const float buttonWidth = buttonBounds.getWidth() / 2.0f;
  auto cbBounds = buttonBounds.removeFromLeft(buttonWidth).reduced(border);
  cancelButton.setBounds(cbBounds.toNearestInt());
  auto sbBounds = buttonBounds.reduced(border);
  saveButton.setBounds(sbBounds.toNearestInt());
}

void SaveDialog::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UXPalette::highlight);
  g.fillRect(fBounds);
  g.setColour(UXPalette::darkBkgnd);
  g.fillRect(fBounds.reduced(3.5f));
}
