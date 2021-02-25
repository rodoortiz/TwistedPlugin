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
    addAndMakeVisible(varComboBox);
    addAndMakeVisible(textButtonL);
    addAndMakeVisible(textButtonR);
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
    files = directoryFile.findChildFiles(File::findFiles, false, "*.wav");
    files.sort();
    for (int i = 0; i < files.size(); i++)
    {
        File file = files.getReference(i);
        varComboBox.addItem(file.getFileNameWithoutExtension(), i+1);
    }

    varComboBox.setJustificationType(Justification::centred);
    varComboBox.setLookAndFeel(&comboBoxLookAndFeel);
    varComboBox.onChange = [&]()
    {
        if(varComboBox.getSelectedId() != 0)
            audioProcessor.loadAFile(files[varComboBox.getSelectedId()-1], drumSamplerType, varComboBox.getSelectedId());
    };


    textButtonL.onClick = [&](){
        if(varComboBox.getSelectedId() > 1)
            varComboBox.setSelectedId(varComboBox.getSelectedId()-1);
        else
            varComboBox.setSelectedId(varComboBox.getNumItems());
    };


    textButtonR.onClick = [&](){
        if(varComboBox.getSelectedId() < varComboBox.getNumItems())
            varComboBox.setSelectedId(varComboBox.getSelectedId()+1);
        else
            varComboBox.setSelectedId(1);
    };
}
