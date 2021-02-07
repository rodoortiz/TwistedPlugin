/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Twisted_pluginAudioProcessorEditor::Twisted_pluginAudioProcessorEditor (Twisted_pluginAudioProcessor& p)
    : AudioProcessorEditor (&p), masterSliders(p), buttonPlayCC(p), buttonStopCC(p), keyboardComponent(p), audioProcessor(p)
{
    imagen = ImageCache::getFromMemory (BinaryData::Twisted_No_Logo_png, BinaryData::Twisted_No_Logo_pngSize);
    imagenComponent.setImage(imagen, RectanglePlacement::stretchToFit);
    addAndMakeVisible(imagenComponent);
    
    logo = ImageCache::getFromMemory (BinaryData::SH_Logo_png, BinaryData::SH_Logo_pngSize);
    logoComponent.setImage(logo, RectanglePlacement(RectanglePlacement::Flags::xLeft));
    addAndMakeVisible(logoComponent);
    
    labelSamplers = LabelSamplerCC::createObjects(p);
    buttonSolos = ButtonSoloCC::createObjects(p);
    waveForms = WaveFormCC::createObjects(p);
    comboBoxes = ComboBoxCC::createObjects(p);
    sliders = ReverbAndBoostSlider::createObjects(p);

    for (int i=0; i<4; i++)
    {
        addAndMakeVisible(waveForms[i].get());
        addAndMakeVisible(labelSamplers[i].get());
        addAndMakeVisible(buttonSolos[i].get());
        addAndMakeVisible(comboBoxes[i].get());
        addAndMakeVisible(sliders[i].get());
    };
    
    addAndMakeVisible(masterSliders);
    
    boostAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "BOOST", *sliders[0]->slider);
    reverbSnrAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REVERB_SNR", *sliders[1]->slider);
    reverbHHAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REVERB_HH", *sliders[2]->slider);
    reverbPercsAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REVERB_PERCS", *sliders[3]->slider);
    
    sliders[0]->slider->setTextValueSuffix(" Hz");

    startTimerHz(30);

    //Play and Stop Add
    addAndMakeVisible(buttonPlayCC);
    addAndMakeVisible(buttonStopCC);

    //MidiKeyboard Add
    addAndMakeVisible(keyboardComponent);

    setSize (1000, 600);
}

Twisted_pluginAudioProcessorEditor::~Twisted_pluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void Twisted_pluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    buttonPlayCC.varTextButton.onClick = [&](){
        buttonPlayCC.varTextButton.setToggleState(true, dontSendNotification);
        buttonPlayCC.varTextButton.setEnabled(false);
        audioProcessor.myPlay();
        buttonStopCC.varTextButton.setToggleState(false, dontSendNotification);
        buttonStopCC.varTextButton.setEnabled(true);
   };
    buttonStopCC.varTextButton.onClick = [&](){
        buttonStopCC.varTextButton.setToggleState(true, dontSendNotification);
        buttonStopCC.varTextButton.setEnabled(false);
        audioProcessor.myStop();
        buttonPlayCC.varTextButton.setToggleState(false, dontSendNotification);
        buttonPlayCC.varTextButton.setEnabled(true);
    };
    
    
    if(audioProcessor.isPlaying)
    {
        if(!audioProcessor.keyboardState.isNoteOn(1, 84) &&
           !audioProcessor.keyboardState.isNoteOn(1, 86) &&
           !audioProcessor.keyboardState.isNoteOn(1, 88) &&
           !audioProcessor.keyboardState.isNoteOn(1, 89) )
        {
        buttonStopCC.varTextButton.setToggleState(true, dontSendNotification);
        buttonStopCC.varTextButton.setEnabled(false);
        buttonPlayCC.varTextButton.setToggleState(false, dontSendNotification);
        buttonPlayCC.varTextButton.setEnabled(true);
            audioProcessor.isPlaying=false; 
        }
    }
}

void Twisted_pluginAudioProcessorEditor::resized()
{
    imagenComponent.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    logoComponent.setBoundsRelative(0.088f, 0.018f, 0.16f, 0.16f);

    for (int i=0; i<4; i++)
    {
        waveForms[i]->setBoundsRelative(waveFormxPos, 0.2f + yPosPlus*i, 0.3f, 0.1f);
        labelSamplers[i]->setBoundsRelative(labelSamplerXpos, 0.225f+ yPosPlus*i, 0.1f, 0.05f);
        buttonSolos[i]->setBoundsRelative(buttonSoloXpos, 0.225f + yPosPlus*i, 0.025f, 0.05f);
        comboBoxes[i]->setBoundsRelative(comboBoxXpos, 0.225f + yPosPlus*i, 0.2f, 0.05f);
        sliders[i]->setBoundsRelative(reverbSliderXpos, 0.19f+yPosPlus*i, 0.06f, 0.13f);
    };

    keyboardComponent.setBoundsRelative(0, 0.8f, 1, 0.2f);
    buttonPlayCC.setBoundsRelative(0.025f, 0.07f, 0.025f, 0.05f);
    buttonStopCC.setBoundsRelative(0.225f, 0.07f, 0.025f, 0.05f);
    masterSliders.setBoundsRelative(masterSlidersXpos, yPosPlus, 0.25f, 0.63f);
    
}

//OWN CODE - TIMER
void Twisted_pluginAudioProcessorEditor::timerCallback()
{
    repaint();
}
