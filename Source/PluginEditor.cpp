/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Twisted_pluginAudioProcessorEditor::Twisted_pluginAudioProcessorEditor (Twisted_pluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
    audioProcessor (p), keyboardComponent(p), buttonPlayCC(p)
{
    imagen = ImageCache::getFromMemory (BinaryData::Twisted_Image_1_png, BinaryData::Twisted_Image_1_pngSize);
    imagenComponent.setImage(imagen, RectanglePlacement::stretchToFit);
    addAndMakeVisible(imagenComponent);
    
    labelSamplers = LabelSamplerCC::createObjects(p);
    buttonSolos = ButtonSoloCC::createObjects(p);
    waveForms = WaveFormCC::createObjects(p);
    comboBoxes = ComboBoxCC::createObjects(p);
    
    for (int i=0; i<4; i++)
    {
        addAndMakeVisible(waveForms[i].get());
        addAndMakeVisible(labelSamplers[i].get());
        addAndMakeVisible(buttonSolos[i].get());
        addAndMakeVisible(comboBoxes[i].get());
    };

    startTimerHz(30);
    
    //MidiKeyboard Add
    addAndMakeVisible(keyboardComponent);

    //Play Add
    addAndMakeVisible(buttonPlayCC);

    setSize (1000, 600);
}

Twisted_pluginAudioProcessorEditor::~Twisted_pluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void Twisted_pluginAudioProcessorEditor::paint (juce::Graphics& g)
{

}

void Twisted_pluginAudioProcessorEditor::resized()
{
    imagenComponent.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    
    for (int i=0; i<4; i++)
    {
        waveForms[i]->setBoundsRelative(waveFormxPos, 0.2f + yPosPlus*i, 0.3f, 0.1f);
        labelSamplers[i]->setBoundsRelative(labelSamplerXpos, 0.225f+ yPosPlus*i, 0.1f, 0.05f);
        buttonSolos[i]->setBoundsRelative(buttonSoloXpos, 0.225f + yPosPlus*i, 0.025f, 0.05f);
        comboBoxes[i]->setBoundsRelative(comboBoxXpos, 0.225f + yPosPlus*i, 0.2f, 0.05f);
    };

    keyboardComponent.setBoundsRelative(0, 0.8f, 1, 0.2f);
    
    buttonPlayCC.setBoundsRelative(0.025f, 0.05f, 0.025f, 0.05f);
}

//OWN CODE - TIMER
void Twisted_pluginAudioProcessorEditor::timerCallback()
{
    repaint();
}

