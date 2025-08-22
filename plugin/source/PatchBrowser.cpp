
#include "PatchBrowser.h"
#include "Identifiers.h"
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
}
