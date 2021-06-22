/*
  ==============================================================================

    PatchBrowser.h
    Created: 22 Jun 2021 2:14:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "ComponentUtil.h"
#include "Color.h"
#include "PluginProcessor.h"

enum class PatchType
{
    bass,
    lead,
    chord,
    pad
};
//keep both of these in the same order so strings can be accessed by int casting the enum value
static const std::vector<juce::String> PatchTypeStrings
{
    "Bass",
    "Lead",
    "Chord",
    "Pad"
};

struct PatchInfo
{
    juce::String name;
    PatchType type;
    PatchInfo(juce::String n, PatchType t) : name(n), type(t)
    {
        
    }
    juce::String getString()
    {
        return PatchTypeStrings[(int)type];
    }
};
struct PatchGroup
{
    PatchGroup(PatchType t) : type(t), typeId(PatchTypeStrings[(int)type])
    {
        
    }
    const PatchType type;
    const juce::String typeId;
    std::vector<juce::File> patches;
    void loadFilesForType(juce::File& folder)
    {
        auto allFiles = folder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true);
        for(auto f : allFiles)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(f);
            if(currentXml != nullptr)
            {
                if(currentXml->hasAttribute("HexPatchType"))
                {
                    auto checkType = currentXml->getStringAttribute("HexPatchType");
                    if(checkType == PatchTypeStrings[(int)type])
                        patches.push_back(f);
                    
                }
            }
        }
    }
    juce::String nameOfLastPatch()
    {
        if(patches.size() > 0)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(patches[patches.size() - 1]);
            if(currentXml->hasAttribute("HexPatchName"))
                return currentXml->getStringAttribute("HexPatchName");
        }
        return "";
    }
};

struct PatchLibrary
{
    PatchLibrary() :
    bassGroup(PatchType::bass),
    leadGroup(PatchType::lead),
    chordGroup(PatchType::chord),
    padGroup(PatchType::pad)
    {
        auto appFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        appFolder.setAsCurrentWorkingDirectory();
        auto patchFolder = appFolder.getChildFile("Hex_Patches");
        if(patchFolder.exists() && patchFolder.isDirectory())
        {
            patchFolder.setAsCurrentWorkingDirectory();
            printf("Folder Found\n");
        }
        else
        {
            patchFolder.createDirectory();
            printf("Folder Created\n");
            patchFolder.setAsCurrentWorkingDirectory();
        }
        presetFolder = patchFolder;
        bassGroup.loadFilesForType(presetFolder);
        leadGroup.loadFilesForType(presetFolder);
        chordGroup.loadFilesForType(presetFolder);
        padGroup.loadFilesForType(presetFolder);
    }
    PatchGroup* getGroup(juce::String& typeStr)
    {
        PatchType type = PatchType::lead;
        int idx = 0;
        for(auto s : PatchTypeStrings)
        {
            if(s == typeStr)
                type = (PatchType)idx;
            ++idx;
        }
        switch(type)
        {
            case PatchType::bass:
            {
                return &bassGroup;
            }
            case PatchType::lead:
            {
                return &leadGroup;
            }
            case PatchType::chord:
            {
                return &chordGroup;
            }
            case PatchType::pad:
            {
                return &padGroup;
            }
        }
    }
    PatchGroup bassGroup;
    PatchGroup leadGroup;
    PatchGroup chordGroup;
    PatchGroup padGroup;
    juce::File presetFolder;
};

class PatchSelector : public juce::ComboBox
{
public:
    PatchSelector() : lib(std::make_unique<PatchLibrary>())
    {
        initialize();
        lastPatchNames = patchNames;
        int colorId = juce::ComboBox::ColourIds::backgroundColourId;
        getLookAndFeel().setColour(colorId, UXPalette::darkGray);
        colorId = juce::TextButton::ColourIds::buttonColourId;
        getLookAndFeel().setColour(colorId, UXPalette::darkGray);
        colorId = juce::PopupMenu::ColourIds::backgroundColourId;
        getLookAndFeel().setColour(colorId, UXPalette::darkGray);
    }
    ~PatchSelector(){}
    void initialize();
    void selectNewest();
    void addPatch(std::unique_ptr<juce::XmlElement>& element, int idNum);
    void reInitList()
    {
        clear();
        lib.reset(new PatchLibrary());
        initialize();
        selectNewest();
    }
    int getIndexWithText(juce::String text)
    {
        for(int i = 0; i < getNumItems(); ++i)
        {
            if(getItemText(i) == text)
                return i;
        }
        return getNumItems() - 1;
    }
    juce::StringArray patchNames;
    juce::StringArray lastPatchNames;
private:
    std::unique_ptr<PatchLibrary> lib;
};

class PatchLoader : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
{
public:
    PatchLoader(HexAudioProcessor* proc, juce::Component* patchDlg);
    ~PatchLoader() {}
    void resized() override;
    void paint(juce::Graphics& g) override
    {
        g.fillAll(UXPalette::darkGray);
    }
    void savePreset(juce::String name, juce::String type);
    void loadPreset(juce::String name);
    void comboBoxChanged(juce::ComboBox* box) override;
    //
    juce::ComboBox* getSelectorBox()
    {
        return &patchSelector;
    }
    juce::String getCurrentPresetName()
    {
        return patchSelector.getText();
    }
    juce::String getCurrentPresetType()
    {
        auto fArray = presetFolder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true);
        juce::String output = "Bass";
        auto pName = getCurrentPresetName();
        for(auto f : fArray)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(f);
            if(currentXml != nullptr)
            {
                if(currentXml->hasAttribute("HexFmPatchType"))
                {
                    auto checkName = currentXml->getStringAttribute("HexFmPatchName");
                    if(checkName == pName)
                    {
                        output = currentXml->getStringAttribute("HexFmPatchType");
                    }
                }
            }
        }
        return output;
    }
    void buttonClicked(juce::Button* button) override
    {
        auto totalItems = patchSelector.getNumItems();
        auto currentIndex = patchSelector.getSelectedItemIndex();
        auto maxIncrease = totalItems - currentIndex;
        if(button == &nextPatchButton && maxIncrease > 0)
        {
            patchSelector.setSelectedItemIndex(currentIndex + 1);
        }
        else if(button == &lastPatchButton && (currentIndex - 1) >= 0)
        {
            patchSelector.setSelectedItemIndex(currentIndex - 1);
        }
        else if(button == &saveButton)
        {
            saveDialogComponent->setEnabled(true);
            saveDialogComponent->setVisible(true);
            saveDialogComponent->toFront(true);
            saveDialogComponent->resized();
        }
    }
    PatchSelector patchSelector;
    
    HexAudioProcessor* const processor;
private:
    juce::File presetFolder;
    juce::StringArray patchNames;
    juce::Component* saveDialogComponent;
    juce::TextButton saveButton;
    juce::TextButton nextPatchButton;
    juce::TextButton lastPatchButton;
    juce::StringArray displayPatchNames;
};

class PatchDialogBox : public juce::Component, juce::Button::Listener
{
public:
    PatchDialogBox(PatchLoader* loader);
    ~PatchDialogBox() {}
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void paint(juce::Graphics& g) override;
    juce::String getNewPatchName()
    {
        return nameField.getText();
    }
    juce::String getNewPatchType()
    {
        return typeBox.getText();
    }
    juce::TextButton savePatchButton;
    juce::TextButton cancelButton;
    juce::ComboBox typeBox;
private:
    PatchLoader* patchLoader;
    juce::TextEditor nameField;
};




