/*
  ==============================================================================

    PatchBrowser.cpp
    Created: 22 Jun 2021 2:14:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "PatchBrowser.h"

void PatchSelector::addPatch(std::unique_ptr<juce::XmlElement> &currentXml, int idNum)
{
    if(currentXml != nullptr)
    {
        if(currentXml->hasAttribute("HexPatchName"))
        {
            auto presetName = currentXml->getStringAttribute("HexPatchName");
            patchNames.add(presetName);
            addItem(presetName, idNum);
        }
    }
}
void PatchSelector::selectNewest()
{
    int newestIndex = 0;
    for(int i = 0; i < patchNames.size(); ++i)
    {
        if(!lastPatchNames.contains(patchNames[i]))
        {
            newestIndex = i;
            setSelectedItemIndex(newestIndex);
            setText(getItemText(newestIndex));
            lastPatchNames = patchNames;
            return;
        }
    }
}

void PatchSelector::initialize()
{
    int nextId = 1;
    patchNames.clear();
    if(lib->bassGroup.patches.size() > 0)
    {
        addSectionHeading("Bass");
        addSeparator();
        for(auto f : lib->bassGroup.patches)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(f);
            addPatch(currentXml, nextId);
            ++nextId;
        }
    }
    
    if(lib->leadGroup.patches.size() > 0)
    {
        addSectionHeading("Lead");
        addSeparator();
        for(auto f : lib->leadGroup.patches)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(f);
            addPatch(currentXml, nextId);
            ++nextId;
        }
    }
    
    if(lib->chordGroup.patches.size() > 0)
    {
        addSectionHeading("Chord");
        addSeparator();
        for(auto f : lib->chordGroup.patches)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(f);
            addPatch(currentXml, nextId);
            ++nextId;
        }
    }
    
    if(lib->padGroup.patches.size() > 0)
    {
        addSectionHeading("Pad");
        addSeparator();
        for(auto f : lib->padGroup.patches)
        {
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(f);
            addPatch(currentXml, nextId);
            ++nextId;
        }
    }
}
//==============================================================================
PatchLoader::PatchLoader(HexAudioProcessor* proc, juce::Component* patchDlg) :
processor(proc),
saveDialogComponent(patchDlg)
{
    auto appFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    auto patchFolder = appFolder.getChildFile("Hex_Patches");
    if(patchFolder.exists() && patchFolder.isDirectory())
    {
        //printf("patch folder exists\n");
        patchFolder.setAsCurrentWorkingDirectory();
    }
    else
    {
        patchFolder.createDirectory();
        patchFolder.setAsCurrentWorkingDirectory();
        //printf("patch folder created\n");
    }
    presetFolder = patchFolder;
    
    //auto fldr = presetFolder.getFileName().toUTF8();
    //printf("Presets are at: %s\n", fldr);
    
    addAndMakeVisible(&patchSelector);
    addAndMakeVisible(&nextPatchButton);
    addAndMakeVisible(&lastPatchButton);
    addAndMakeVisible(&saveButton);
    
    nextPatchButton.addListener(this);
    lastPatchButton.addListener(this);
    saveButton.addListener(this);
    patchSelector.addListener(this);
    
    nextPatchButton.setButtonText(">");
    lastPatchButton.setButtonText("<");
    saveButton.setButtonText("Save");
}

void PatchLoader::resized()
{
    auto bounds = getLocalBounds();
    auto halfY = bounds.getHeight() / 2;
    auto upperBounds = bounds.removeFromTop(halfY);
    lastPatchButton.setBounds(upperBounds.removeFromLeft(halfY));
    nextPatchButton.setBounds(upperBounds.removeFromLeft(halfY));
    auto saveWidth = saveButton.getBestWidthForHeight(halfY);
    saveButton.setBounds(upperBounds.removeFromRight(saveWidth));
    auto cushion = halfY / 6;
    patchSelector.setBounds(bounds.reduced(cushion));
}

void PatchLoader::savePreset(juce::String name, juce::String type)
{
    auto fileName = juce::File::createLegalFileName(name);
    if(presetFolder.exists())
    {
        auto file = presetFolder.getChildFile(fileName);
        bool isNew = false;
        if(!file.existsAsFile())
        {
            file.create();
            isNew = true;
        }
        auto state = processor->tree.copyState();
        std::unique_ptr<juce::XmlElement> xml = state.createXml();
        xml->setAttribute("HexPatchName", name);
        xml->setAttribute("HexPatchType", type);
        xml->writeTo(file);
        patchSelector.reInitList();
    }
}
void PatchLoader::loadPreset(juce::String presetName)
{
    auto presetFiles = presetFolder.findChildFiles(juce::File::findFiles, true);
    if(presetFiles.size() > 0)
    {
        for(int i = 0; i < presetFiles.size(); ++i)
        {
            auto currentFile = presetFiles.getUnchecked(i);
            std::unique_ptr<juce::XmlElement> currentXml = juce::parseXML(currentFile);
            if(currentXml != nullptr)
            {
                if(currentXml->hasAttribute("HexPatchName"))
                {
                    auto checkName = currentXml->getStringAttribute("HexPatchName");
                    if(checkName == presetName)
                    {
                        auto* vTree = &processor->tree;
                        vTree->replaceState(juce::ValueTree::fromXml(*currentXml));
                    }
                }
            }
        }
    }
}
    
void PatchLoader::comboBoxChanged(juce::ComboBox* box)
{
    if(box == &patchSelector)
    {
        loadPreset(patchSelector.getText());
    }
}

PatchDialogBox::PatchDialogBox(PatchLoader* loader) : patchLoader(loader)
{
    savePatchButton.addListener(this);
    cancelButton.addListener(this);
    addAndMakeVisible(&nameField);
    nameField.setMultiLine(false);
    nameField.setTextToShowWhenEmpty("Patch Name", UXPalette::lightGray);
    nameField.setPopupMenuEnabled(false);
    
    addAndMakeVisible(&savePatchButton);
    savePatchButton.setButtonText("Save patch");
    
    addAndMakeVisible(&cancelButton);
    cancelButton.setButtonText("Cancel");
    
    addAndMakeVisible(&typeBox);
    int idx = 1;
    for(auto t : PatchTypeStrings)
    {
        typeBox.addItem(t, idx);
        ++idx;
    }
    typeBox.setSelectedId(1);
}

void PatchDialogBox::paint(juce::Graphics &g)
{
    g.fillAll(UXPalette::darkBkgnd);
    auto cushion = getHeight() / 14.0f;
    auto lightArea = getLocalBounds().toFloat().reduced(cushion);
    g.setColour(UXPalette::darkGray);
    g.fillRect(lightArea);
    g.setColour(juce::Colours::white);
    int w = getWidth() / 18;
    int h = getHeight() / 8;
    auto textBox = juce::Rectangle<int> {w, h, 8 * w, 2 * h};
    g.drawText("Patch Name:", textBox, juce::Justification::left);
}


void PatchDialogBox::resized()
{
    int w = getWidth() / 18;
    int h = getHeight() / 12;
    
    nameField.setBounds(w, 3.5 * h, 16 * w, h);
    savePatchButton.setBounds(8.5 * w, 5 * h, 4 * w, 1.5 * h);
    cancelButton.setBounds(13 * w, 5 * h, 3 * w, 1.5 * h);
    typeBox.setBounds(w, 5 * h, 6 * w, 1.5 * h);
    auto name = patchLoader->getCurrentPresetName();
    auto type = patchLoader->getCurrentPresetType();
    nameField.setText(name);
    for(int i = 0; i < typeBox.getNumItems(); ++i)
    {
        if(typeBox.getItemText(i) == type)
            typeBox.setSelectedItemIndex(i);
    }
}

void PatchDialogBox::buttonClicked(juce::Button *button)
{
    if(button == &savePatchButton)
    {
        auto patchName = nameField.getText();
        patchLoader->savePreset(patchName, typeBox.getText());
    }
        nameField.clear();
        setEnabled(false);
        setVisible(false);
}


