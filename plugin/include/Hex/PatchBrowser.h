

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
  std::unique_ptr<ParamAttachment> pAttach;
  void paramCallback(float fValue);
  void updateButtonEnablement();
  int selectedPatchIdx = -1;

public:
  PatchComboBox(HexState* s);
  ~PatchComboBox() override;
  String getCurrentPatchName() const;
  void comboBoxChanged(juce::ComboBox* cb) override;
  void resized() override;
  void newPatchSaved(const String& name) override;
  void existingPatchSaved(const String& name) override;
};
//==========================

class PatchLoader : public Component {
private:
  // PatchComboBox cb;
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

class LoadDialog : public Component {
private:
  HexState* const state;

public:
  LoadDialog(HexState* s);
};
