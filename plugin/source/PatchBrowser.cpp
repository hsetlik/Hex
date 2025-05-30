
#include "PatchBrowser.h"
#include "Identifiers.h"

PatchComboBox::PatchComboBox(HexState* s) : state(s) {
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
  if (idx > -1 && idx) {
    // TODO: call the parent to load the appropriate
    // patch data here
    updateButtonEnablement();
  }
}

void PatchComboBox::updateButtonEnablement() {
  auto currentIdx = cb.getSelectedItemIndex();
  lButton.setEnabled(currentIdx > 0);
  rButton.setEnabled(currentIdx < cb.getNumItems() - 1);
}

void PatchComboBox::comboBoxChanged(juce::ComboBox* cb) {
  auto newIdx = cb->getSelectedItemIndex();
}
