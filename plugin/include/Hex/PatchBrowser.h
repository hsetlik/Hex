

#pragma once
#include "HexState.h"
#include "SymbolButton.h"

class PatchBrowserParent : public Component {
public:
  PatchBrowserParent() {}
  virtual ~PatchBrowserParent() {}
  virtual void loadPatch(const String& name) = 0;
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

public:
  PatchComboBox(HexState* s);
  ~PatchComboBox() override;
  void comboBoxChanged(juce::ComboBox* cb) override;
  void resized() override;
  void newPatchSaved(const String& name) override;
  void existingPatchSaved(const String& name) override;
};
