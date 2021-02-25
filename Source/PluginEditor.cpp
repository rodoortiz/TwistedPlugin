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
    labelsEffects = LabelsEffects::createObjects(p);
    buttonSolos = ButtonSoloCC::createObjects(p);
    waveForms = WaveFormCC::createObjects(p);
    comboBoxes = ComboBoxCC::createObjects(p);
    sliders = ReverbAndBoostSlider::createObjects(p);

    for (int i=0; i<4; i++)
    {
        addAndMakeVisible(waveForms[i].get());
        addAndMakeVisible(labelSamplers[i].get());
        addAndMakeVisible(labelsEffects[i].get());
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
    sliders[1]->slider->setTextValueSuffix(" %");
    sliders[2]->slider->setTextValueSuffix(" %");
    sliders[3]->slider->setTextValueSuffix(" %");

    buttonPlayCC.varTextButton.onClick = [&]()
    {
        buttonPlayCC.varTextButton.setToggleState(true, dontSendNotification);
        buttonPlayCC.varTextButton.setEnabled(false);
        audioProcessor.myPlay();
        buttonStopCC.varTextButton.setToggleState(false, dontSendNotification);
        buttonStopCC.varTextButton.setEnabled(true);
    };

    buttonStopCC.varTextButton.onClick = [&]()
    {
        buttonStopCC.varTextButton.setToggleState(true, dontSendNotification);
        buttonStopCC.varTextButton.setEnabled(false);
        audioProcessor.myStop();
        buttonPlayCC.varTextButton.setToggleState(false, dontSendNotification);
        buttonPlayCC.varTextButton.setEnabled(true);
    };


    startTimerHz(30);

    //Play and Stop Add
    addAndMakeVisible(buttonPlayCC);
    addAndMakeVisible(buttonStopCC);

    //MidiKeyboard Add
    addAndMakeVisible(keyboardComponent);

    setSize (1000, 600);

    //RequestAndUpdateComboboxAncientValues
    if(audioProcessor.getComboBoxInfo(1)!=0)
        comboBoxes[0]->varComboBox.setSelectedId(audioProcessor.getComboBoxInfo(1));
    if(audioProcessor.getComboBoxInfo(2)!=0)
        comboBoxes[1]->varComboBox.setSelectedId(audioProcessor.getComboBoxInfo(2));
    if(audioProcessor.getComboBoxInfo(3)!=0)
        comboBoxes[2]->varComboBox.setSelectedId(audioProcessor.getComboBoxInfo(3));
    if(audioProcessor.getComboBoxInfo(4)!=0)
        comboBoxes[3]->varComboBox.setSelectedId(audioProcessor.getComboBoxInfo(4));

    //RequestAndUpdateSoloAncienteValues
    buttonSolos[0]->varTextButton.setToggleState(audioProcessor.getSoloInfo(1), dontSendNotification);
    buttonSolos[1]->varTextButton.setToggleState(audioProcessor.getSoloInfo(2), dontSendNotification);
    buttonSolos[2]->varTextButton.setToggleState(audioProcessor.getSoloInfo(3), dontSendNotification);
    buttonSolos[3]->varTextButton.setToggleState(audioProcessor.getSoloInfo(4), dontSendNotification);

    //RequestAndUpdateAPVTSAncienteValues
    sliders[0]->slider->setValue(audioProcessor.getAPVTSInfo(1));
    sliders[1]->slider->setValue(audioProcessor.getAPVTSInfo(2));
    sliders[2]->slider->setValue(audioProcessor.getAPVTSInfo(3));
    sliders[3]->slider->setValue(audioProcessor.getAPVTSInfo(4));
    masterSliders.lowEQSlider->setValue(audioProcessor.getAPVTSInfo(5));
    masterSliders.highEQSlider->setValue(audioProcessor.getAPVTSInfo(6));
    masterSliders.filterSlider->setValue(audioProcessor.getAPVTSInfo(7));
    masterSliders.gainSlider->setValue(audioProcessor.getAPVTSInfo(8));

    //Login Component
    String response = loginComponent.readJSON();
    var parsedJSON = JSON::parse(response);

    int actualTimeInSec = getMyTime();
    int iat = int(parsedJSON["iat"]);
    int exp = int(parsedJSON["exp"]);

    if(!parsedJSON["vst_access"])
        addAndMakeVisible(loginComponent);
    else {
        if(!(actualTimeInSec >= iat && actualTimeInSec <= exp))
            addAndMakeVisible(loginComponent);
    }
}

Twisted_pluginAudioProcessorEditor::~Twisted_pluginAudioProcessorEditor()
{
    //SaveComboboxValues
    audioProcessor.setComboBoxInfo(1, comboBoxes[0]->varComboBox.getSelectedId());
    audioProcessor.setComboBoxInfo(2, comboBoxes[1]->varComboBox.getSelectedId());
    audioProcessor.setComboBoxInfo(3, comboBoxes[2]->varComboBox.getSelectedId());
    audioProcessor.setComboBoxInfo(4, comboBoxes[3]->varComboBox.getSelectedId());

    //SaveSoloValues
    audioProcessor.setSoloInfo(1, buttonSolos[0]->varTextButton.getToggleState());
    audioProcessor.setSoloInfo(2, buttonSolos[1]->varTextButton.getToggleState());
    audioProcessor.setSoloInfo(3, buttonSolos[2]->varTextButton.getToggleState());
    audioProcessor.setSoloInfo(4, buttonSolos[3]->varTextButton.getToggleState());

    //SaveAPVTSValues
    audioProcessor.setAPVTSInfo(1, sliders[0]->slider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(2, sliders[1]->slider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(3, sliders[2]->slider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(4, sliders[3]->slider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(5, masterSliders.lowEQSlider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(6, masterSliders.highEQSlider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(7, masterSliders.filterSlider->getValueObject().getValue());
    audioProcessor.setAPVTSInfo(8, masterSliders.gainSlider->getValueObject().getValue());

    stopTimer();
}

//==============================================================================
void Twisted_pluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    loginComponent.setAlpha(0.9f);
}

void Twisted_pluginAudioProcessorEditor::resized()
{
    imagenComponent.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    logoComponent.setBoundsRelative(0.088f, 0.018f, 0.16f, 0.16f);

    for (int i=0; i<4; i++)
    {
        waveForms[i]->setBoundsRelative(waveFormxPos, 0.2f + yPosPlus*i, 0.3f, 0.1f);
        labelSamplers[i]->setBoundsRelative(labelSamplerXpos, 0.225f+ yPosPlus*i, 0.1f, 0.05f);
        labelsEffects[i]->setBoundsRelative(labelEffectsXpos, 0.325f+yPosPlus*i, 0.06f, 0.02f);
        buttonSolos[i]->setBoundsRelative(buttonSoloXpos, 0.225f + yPosPlus*i, 0.025f, 0.05f);
        comboBoxes[i]->setBoundsRelative(comboBoxXpos, 0.225f + yPosPlus*i, 0.2f, 0.05f);
        sliders[i]->setBoundsRelative(reverbSliderXpos, 0.19f+yPosPlus*i, 0.06f, 0.13f);
    };

    keyboardComponent.setBoundsRelative(0, 0.8f, 1, 0.2f);
    buttonPlayCC.setBoundsRelative(0.025f, 0.07f, 0.025f, 0.05f);
    buttonStopCC.setBoundsRelative(0.225f, 0.07f, 0.025f, 0.05f);
    masterSliders.setBoundsRelative(masterSlidersXpos, yPosPlus, 0.25f, 0.63f);

    loginComponent.setSize(375, 300);
    loginComponent.setCentreRelative(0.25f, 0.28f);


}

//OWN CODE - TIMER
void Twisted_pluginAudioProcessorEditor::timerCallback()
{
    repaint();
    int cleanValue = audioProcessor.triggerCleanerCombobox();
    if (cleanValue!=0)
        comboBoxes[cleanValue-1]->varComboBox.setSelectedId(0);
    if (audioProcessor.buttonsResetProcess)
    {
        buttonStopCC.varTextButton.setToggleState(true, dontSendNotification);
        buttonStopCC.varTextButton.setEnabled(false);
        buttonPlayCC.varTextButton.setToggleState(false, dontSendNotification);
        buttonPlayCC.varTextButton.setEnabled(true);
        audioProcessor.buttonsResetProcess = false;
    }
}

int Twisted_pluginAudioProcessorEditor::getMyTime()
{
    return (int)(Time::currentTimeMillis() / 1000);
}
