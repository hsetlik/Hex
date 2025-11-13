
#include "PatchBrowser.h"
#include "Color.h"
#include "FileSystem.h"
#include "Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

#define MODAL_INSET 6.5f
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
    parent->openLoadDialog(patchName);
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
  auto fBounds = getLocalBounds().toFloat().reduced(MODAL_INSET);
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
  g.fillRect(fBounds.reduced(MODAL_INSET));
}

//===========================================================================

PatchInfoBar::PatchInfoBar(const patch_info_t& _info) : info(_info) {}

bool PatchInfoBar::isSelected() const {
  auto* list = findParentComponentOfClass<PatchInfoList>();
  jassert(list != nullptr);
  return list->barIsSelected(*this);
}

void PatchInfoBar::mouseUp(const juce::MouseEvent& e) {
  if (e.mouseWasClicked() && isMouseOver()) {
    auto* parent = findParentComponentOfClass<PatchInfoList>();
    jassert(parent != nullptr);
    parent->setSelected(this);
  }
}

void PatchInfoBar::mouseDoubleClick(const juce::MouseEvent& e) {
  juce::ignoreUnused(e);
  // TODO: this should load the double-clicked patch and
  // close the modal dialog
}

static AttString buildPatchBatAttString(const String& text) {
  AttString aStr(text);
  // TODO font and formatting stuff gets handled here
  return aStr;
}

void PatchInfoBar::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  // 1. fill the background
  if (!isSelected()) {
    g.setColour(UXPalette::darkGray);
    g.fillRect(fBounds);
  } else {
    g.setColour(UXPalette::highlight);
    g.fillRect(fBounds);
    g.setColour(UXPalette::darkGray);
    g.fillRect(fBounds.reduced(2.0f));
  }
  fBounds = fBounds.reduced(2.0f);
  // 2. divide up the bar's area into sections
  const float dX = fBounds.getWidth() / 10.0f;
  const float nWidth = 4.0f * dX;
  const float aWidth = 3.0f * dX;
  const float cWidth = 3.0f * dX;
  // 3. draw each string and the two dividers
  static const float dividerWidth = 4.0f;
  auto textColor = isSelected() ? UXPalette::highlight : UXPalette::lightGray;

  // name-----
  auto nBounds = fBounds.removeFromLeft(nWidth);
  auto nDiv = nBounds.removeFromRight(dividerWidth);
  g.setColour(textColor);
  g.fillRect(nDiv);
  auto nStr = buildPatchBatAttString(info.name);
  nStr.setColour(textColor);
  nStr.draw(g, nBounds);

  // author----
  auto aBounds = fBounds.removeFromLeft(aWidth);
  auto aDiv = aBounds.removeFromRight(dividerWidth);
  g.fillRect(aDiv);
  auto aStr = buildPatchBatAttString(info.author);
  aStr.setColour(textColor);
  aStr.draw(g, aBounds);

  // category----
  auto cBounds = fBounds;
  auto cStr = buildPatchBatAttString(patchTypeNames[info.type]);
  cStr.setColour(textColor);
  cStr.draw(g, cBounds);
  // g.setColour(UXPalette::highlight);
  // g.fillRect(getLocalBounds());
}

//===========================================================================
namespace PatchSort {
bool compareNames(const patch_info_t& a,
                  const patch_info_t& b,
                  bool ascending) {
  int comp = a.name.compare(b.name);
  // flip the comparison if we're descending
  if (!ascending)
    comp *= -1;
  if (comp < 0) {
    return true;
  } else if (comp > 0) {
    return false;
  }
  return compareCategories(a, b, ascending);
}

bool compareAuthors(const patch_info_t& a,
                    const patch_info_t& b,
                    bool ascending) {
  int comp = a.author.compare(b.author);
  // flip the comparison if we're descending
  if (!ascending)
    comp *= -1;
  if (comp < 0) {
    return true;
  } else if (comp > 0) {
    return false;
  }
  return compareNames(a, b, ascending);
}

bool compareCategories(const patch_info_t& a,
                       const patch_info_t& b,
                       bool ascending) {
  String aCateg = patchTypeNames[a.type];
  String bCateg = patchTypeNames[b.type];
  int comp = aCateg.compare(bCateg);
  if (!ascending)
    comp *= -1;
  if (comp < 0) {
    return true;
  } else if (comp > 0) {
    return false;
  }
  return compareNames(a, b, ascending);
}
}  // namespace PatchSort
//===========================================================================

PatchInfoList::PatchInfoList(HexState* s) : state(s) {
  auto patchList = state->patchLib.getAllPatches();
  for (auto& p : patchList) {
    patchBars.add(new PatchInfoBar(p));
    addAndMakeVisible(patchBars.getLast());
  }
  if (patchBars.size() > 0) {
    selectedBar = patchBars.getFirst();
  }
}

std::vector<PatchInfoBar*> PatchInfoList::getBarList() const {
  std::vector<PatchInfoBar*> list = {};
  for (auto* b : patchBars) {
    list.push_back(b);
  }
  return list;
}

std::vector<PatchInfoBar*> PatchInfoList::barsSortedBy(PatchSortModeE mode,
                                                       bool ascending) const {
  auto bars = getBarList();
  switch (mode) {
    case sName:
      std::sort(bars.begin(), bars.end(),
                [ascending](const PatchInfoBar* a, const PatchInfoBar* b) {
                  return PatchSort::compareNames(a->info, b->info, ascending);
                });
      break;
    case sAuthor:
      std::sort(bars.begin(), bars.end(),
                [ascending](const PatchInfoBar* a, const PatchInfoBar* b) {
                  return PatchSort::compareAuthors(a->info, b->info, ascending);
                });
      break;
    case sCategory:
      std::sort(bars.begin(), bars.end(),
                [ascending](const PatchInfoBar* a, const PatchInfoBar* b) {
                  return PatchSort::compareCategories(a->info, b->info,
                                                      ascending);
                });
      break;
    default:
      jassert(false);
      break;
  }
  return bars;
}

String PatchInfoList::selectedPatchName() const {
  jassert(selectedBar != nullptr);
  return selectedBar->info.name;
}

void PatchInfoList::resized() {
  const int width = getWidth();
  const int height = 35;
  const int x = 0;
  int y = 0;
  auto* parent = findParentComponentOfClass<LoadDialog>();
  jassert(parent != nullptr);
  auto list = barsSortedBy(parent->getSortMode(), parent->getAscending());
  for (size_t i = 0; i < list.size(); ++i) {
    list[i]->setBounds(x, y, width, height);
    y += height;
  }
}

void PatchInfoList::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UXPalette::highlight);
  g.fillRect(fBounds);
}

void PatchInfoList::setSelectedName(const String& name) {
  setSelected(barForName(name));
}

PatchInfoBar* PatchInfoList::barForName(const String& name) const {
  for (auto* p : patchBars) {
    if (p->info.name == name)
      return p;
  }
  jassert(false);
  return nullptr;
}

//-------------------------------------------------------
PatchColumnTop::PatchColumnTop(const PatchSortModeE& _mode) : mode(_mode) {}

bool PatchColumnTop::isSelected() const {
  auto* parent = findParentComponentOfClass<LoadDialog>();
  jassert(parent != nullptr);
  return parent->getSortMode() == mode;
}

void PatchColumnTop::paint(juce::Graphics& g) {
  // grip the colors
  auto fillColor = isSelected() ? UXPalette::highlight : UXPalette::darkBkgnd;
  auto strokeColor = isSelected() ? UXPalette::darkBkgnd : UXPalette::highlight;
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(strokeColor);
  g.fillRect(fBounds);
  g.setColour(fillColor);
  g.fillRect(fBounds.reduced(3.0f));
  fBounds = fBounds.reduced(3.0f);
  auto aStr = buildPatchBatAttString(getColumnText());
  aStr.setColour(strokeColor);
  aStr.draw(g, fBounds);
}

String PatchColumnTop::getColumnText() const {
  switch (mode) {
    case sName:
      return "Name";
      break;
    case sAuthor:
      return "Author";
      break;
    case sCategory:
      return "Category";
      break;
  }
  jassert(false);
  return "Name";
}

void PatchColumnTop::mouseUp(const juce::MouseEvent& e) {
  if (e.mouseWasClicked()) {
    if (isSelected()) {
      isAscending = !isAscending;
    }
    auto* parent = findParentComponentOfClass<LoadDialog>();
    jassert(parent != nullptr);
    parent->setSortMode(mode, isAscending);
  }
}
//------------------------------------------------------------------

void LoadDialog::setSortMode(PatchSortModeE _mode, bool _ascending) {
  currentMode = _mode;
  sortAscending = _ascending;
  repaint();
}

LoadDialog::LoadDialog(HexState* s)
    : state(s),
      infoList(s),
      nameCT(sName),
      authorCT(sAuthor),
      categCT(sCategory) {
  // set up viewport
  vp.setViewedComponent(&infoList, false);
  vp.setViewPosition(0, 0);
  vp.setInterceptsMouseClicks(true, true);
  vp.setRepaintsOnMouseActivity(true);
  addAndMakeVisible(vp);
  infoList.resized();

  // set up headers
  addAndMakeVisible(nameCT);
  addAndMakeVisible(authorCT);
  addAndMakeVisible(categCT);

  // load and cancel buttons
  loadBtn.setButtonText("Load");
  loadBtn.onClick = [this]() {
    state->patchLib.loadPatch(&state->mainTree, state->patchTree,
                              infoList.selectedPatchName());
    auto* parent = getBrowserParent();
    parent->closeModal();
  };
  loadBtn.setEnabled(state->patchLib.getNumPatches() > 0);
  addAndMakeVisible(loadBtn);

  cancelBtn.setButtonText("Cancel");
  cancelBtn.onClick = [this]() {
    auto* parent = getBrowserParent();
    parent->closeModal();
  };
  addAndMakeVisible(cancelBtn);
}

PatchBrowserParent* LoadDialog::getBrowserParent() const {
  auto* parent = findParentComponentOfClass<PatchBrowserParent>();
  jassert(parent != nullptr);
  return parent;
}

void LoadDialog::resized() {
  infoList.resized();
  auto fBounds = getLocalBounds().toFloat().reduced(MODAL_INSET);
  const float topHeight = fBounds.getHeight() / 11.0f;
  const float dX = fBounds.getWidth() / 10.0f;
  auto cBounds = fBounds.removeFromTop(topHeight);
  auto nBounds = cBounds.removeFromLeft(4.0f * dX);
  auto aBounds = cBounds.removeFromLeft(3.0f * dX);

  nameCT.setBounds(nBounds.toNearestInt());
  authorCT.setBounds(aBounds.toNearestInt());
  categCT.setBounds(cBounds.toNearestInt());

  auto buttonArea = fBounds.removeFromBottom(fBounds.getHeight() / 8.0f);
  vp.setBounds(fBounds.toNearestInt());
  buttonArea = buttonArea.withSizeKeepingCentre(buttonArea.getWidth() * 0.75f,
                                                buttonArea.getHeight());
  auto cancelBounds =
      buttonArea.removeFromLeft(buttonArea.getWidth() / 2.0f).reduced(2.5f);
  auto loadBounds = buttonArea.reduced(2.5f);
  cancelBtn.setBounds(cancelBounds.toNearestInt());
  loadBtn.setBounds(loadBounds.toNearestInt());
}

void LoadDialog::initializeFor(const String& patchName) {
  infoList.setSelectedName(patchName);
  resized();
}

void LoadDialog::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UXPalette::highlight);
  g.fillRect(fBounds);
  g.fillRect(vp.getLocalBounds());
  g.setColour(UXPalette::darkBkgnd);
  g.fillRect(fBounds.reduced(MODAL_INSET));
}
