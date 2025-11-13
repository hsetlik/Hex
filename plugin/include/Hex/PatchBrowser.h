

#pragma once
#include "HexState.h"
#include "SymbolButton.h"
#include "FileSystem.h"

class PatchBrowserParent : public Component {
public:
  PatchBrowserParent() {}
  virtual ~PatchBrowserParent() {}
  virtual void openSaveDialog(const String& name) = 0;
  virtual void openLoadDialog(const String& name) = 0;
  virtual void closeModal() = 0;
};

//==========================

class PatchComboBox : public Component,
                      public juce::ComboBox::Listener,
                      public PatchLibrary::Listener {
private:
  HexState* const state;
  juce::ComboBox cb;
  LeftButton lButton;
  RightButton rButton;
  void updateButtonEnablement();

public:
  PatchComboBox(HexState* s);
  ~PatchComboBox() override;
  void comboBoxChanged(juce::ComboBox* cb) override;
  void resized() override;
  void newPatchSaved(const String& name) override;
  void existingPatchSaved(const String& name) override;
  void existingPatchLoaded(const String& name) override;
};
//==========================

class PatchLoader : public Component {
private:
  PatchComboBox cb;
  juce::TextButton saveBtn;
  juce::TextButton loadBtn;
  PatchBrowserParent* getBrowserParent();

public:
  PatchLoader(HexState* s);
  void resized() override;
};

//==========================

class SaveDialog : public Component, public juce::TextEditor::Listener {
private:
  HexState* const state;
  // children
  juce::Label nameLabel;
  juce::Label authorLabel;
  juce::Label typeLabel;
  juce::TextEditor nameEditor;
  juce::TextEditor authEditor;
  juce::ComboBox typeBox;

  juce::TextButton nSaveButton;
  juce::TextButton eSaveButton;
  juce::TextButton cancelButton;

  // helpers for saving
  patch_info_t getCurrentInfo() const;
  bool isPatchLegal() const;
  void saveAndClose();

public:
  SaveDialog(HexState* s);
  ~SaveDialog() override;
  void textEditorTextChanged(juce::TextEditor& ed) override;
  // this gets called when we make the save dialog visible
  void initializeFor(const String& patchName);
  void resized() override;
  void paint(juce::Graphics& g) override;
};

//====================================================
class PatchInfoBar : public Component {
public:
  const patch_info_t info;
  PatchInfoBar(const patch_info_t& _info);
  bool isSelected() const;
  void paint(juce::Graphics& g) override;
  // mouse callbacks
  void mouseUp(const juce::MouseEvent& e) override;
  void mouseDoubleClick(const juce::MouseEvent& e) override;
};

enum PatchSortModeE { sName, sAuthor, sCategory };

// helpers for our sorting labmdas
namespace PatchSort {
bool compareNames(const patch_info_t& a,
                  const patch_info_t& b,
                  bool ascending = true);
bool compareAuthors(const patch_info_t& a,
                    const patch_info_t& b,
                    bool ascending = true);
bool compareCategories(const patch_info_t& a,
                       const patch_info_t& b,
                       bool ascending = true);
}  // namespace PatchSort

class PatchInfoList : public Component {
private:
  HexState* const state;
  juce::OwnedArray<PatchInfoBar> patchBars;
  PatchInfoBar* selectedBar = nullptr;
  // helper for getting an unsorted list of all the patches
  std::vector<PatchInfoBar*> getBarList() const;
  // the sorting happens here
  std::vector<PatchInfoBar*> barsSortedBy(PatchSortModeE mode,
                                          bool ascending) const;
  PatchInfoBar* barForName(const String& name) const;

public:
  PatchInfoList(HexState* s);
  bool barIsSelected(const PatchInfoBar& bar) const {
    return &bar == selectedBar;
  }
  void setSelected(PatchInfoBar* bar) { selectedBar = bar; }
  void setSelectedName(const String& name);
  String selectedPatchName() const;
  void resized() override;
  void paint(juce::Graphics& g) override;
};

// buttons at the top of each column for controlling the way patches are sorted
class PatchColumnTop : public Component {
private:
  const PatchSortModeE mode;
  bool isAscending = true;
  bool isSelected() const;
  String getColumnText() const;

public:
  PatchColumnTop(const PatchSortModeE& _mode);
  void paint(juce::Graphics& g) override;
  void mouseUp(const juce::MouseEvent& e) override;
};

class LoadDialog : public Component {
private:
  HexState* const state;
  PatchSortModeE currentMode = sName;
  bool sortAscending = true;

  juce::Viewport vp;
  PatchInfoList infoList;
  PatchColumnTop nameCT;
  PatchColumnTop authorCT;
  PatchColumnTop categCT;

  juce::TextButton loadBtn;
  juce::TextButton cancelBtn;

  PatchBrowserParent* getBrowserParent() const;

public:
  LoadDialog(HexState* s);
  void setSortMode(PatchSortModeE _mode, bool _ascending);
  PatchSortModeE getSortMode() const { return currentMode; }
  bool getAscending() const { return sortAscending; }
  void initializeFor(const String& patchName);
  void resized() override;
  void paint(juce::Graphics& g) override;
};
