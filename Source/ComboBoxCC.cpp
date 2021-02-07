/*
  ==============================================================================

    ComboBoxCC.cpp
    Created: 11 Nov 2020 2:03:17pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ComboBoxCC.h"

//==============================================================================
ComboBoxCC::ComboBoxCC(Twisted_pluginAudioProcessor& p) : audioProcessor(p)
{
    
}

ComboBoxCC::~ComboBoxCC()
{
    varComboBox.setLookAndFeel(0);
}

std::vector<std::unique_ptr<ComboBoxCC>> ComboBoxCC::createObjects(Twisted_pluginAudioProcessor& p)
{
    auto v = std::vector<std::unique_ptr<ComboBoxCC>>();
    
    for (auto i = 0; i < 4; i++)
    {
        v.emplace_back(std::make_unique<ComboBoxCC>(p));
        v[i]->setDrumSamplerType(i+1);
        if(i==0)
            v[i]->setDrumSamplerPath(FileManager::getSampleDirectory(FileManager::SamplesFolder::Kick));
        if(i==1)
            v[i]->setDrumSamplerPath(FileManager::getSampleDirectory(FileManager::SamplesFolder::Snare));
        if(i==2)
            v[i]->setDrumSamplerPath(FileManager::getSampleDirectory(FileManager::SamplesFolder::HiHats));
        if(i==3)
            v[i]->setDrumSamplerPath(FileManager::getSampleDirectory(FileManager::SamplesFolder::Percussion));
    };
    
    return v;
}

void ComboBoxCC::paint (juce::Graphics& g)
{
    if(audioProcessor.mySamplerSoundExists(drumSamplerType))
    {
        if(audioProcessor.myGetAudioLoadedBy(drumSamplerType) == 1)
            varComboBox.setSelectedId(0);
        audioProcessor.mySetAudioLoadedBy(drumSamplerType, 0);
    }
}

void ComboBoxCC::resized()
{
    varComboBox.setBoundsRelative(0.15, 0, 0.7, 1);
    textButtonL.setBoundsRelative(0, 0, 0.15, 1);
    textButtonR.setBoundsRelative(0.85, 0, 0.15, 1);
}

//-----

void ComboBoxCC::setDrumSamplerType(int varInt)
{
    drumSamplerType = varInt;
}

void ComboBoxCC::drumSamplerPath(File directoryFile)
{

    directoryFile.findChildFiles(files, File::findFiles, false);
    int itemCount = 0;
    for (int i = 0; i < files.size(); i++)
    {
        File file = files.getReference(i);
        if(file.getFileExtension() == ".wav")
        {
            itemCount++;
            varComboBox.addItem(file.getFileNameWithoutExtension(), itemCount);
        }
    }
    
    varComboBox.setJustificationType(Justification::centred);
    varComboBox.setLookAndFeel(&comboBoxLookAndFeel);
    varComboBox.onChange = [&]()
    {
        if(varComboBox.getSelectedId() != 0)
            audioProcessor.loadAFile(files[varComboBox.getSelectedId()], drumSamplerType, 2);
    };
    addAndMakeVisible(varComboBox);
    
    textButtonL.onClick = [&](){
        if(varComboBox.getSelectedId() > 1)
            varComboBox.setSelectedId(varComboBox.getSelectedId()-1);
        else
            varComboBox.setSelectedId(varComboBox.getNumItems());
    };
    addAndMakeVisible(textButtonL);
    
    textButtonR.onClick = [&](){
        if(varComboBox.getSelectedId() < varComboBox.getNumItems())
            varComboBox.setSelectedId(varComboBox.getSelectedId()+1);
        else
            varComboBox.setSelectedId(1);
    };
    addAndMakeVisible(textButtonR);
    
    //varComboBox.setSelectedId(1);
}

