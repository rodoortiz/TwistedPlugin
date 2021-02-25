/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveFormCC.h"
#include "ComboBoxCC.h"
#include "LabelSamplerCC.h"
#include "ButtonSoloCC.h"
#include "MidiKeyboard.h"
#include "ButtonPlayCC.h"
#include "ButtonStopCC.h"
#include <array>
#include "ReverbAndBoostSlider.h"
#include "MasterSliders.h"
#include "LoginComponent.h"

//==============================================================================
/**
*/
class Twisted_pluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            public Timer
{
public:
    Twisted_pluginAudioProcessorEditor (Twisted_pluginAudioProcessor&);
    ~Twisted_pluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    int getMyTime();
        
private:
    Image imagen, logo;
    ImageComponent imagenComponent, logoComponent;
    
    std::vector<std::unique_ptr<WaveFormCC>> waveForms;
    std::vector<std::unique_ptr<ComboBoxCC>> comboBoxes;
    std::vector<std::unique_ptr<LabelSamplerCC>> labelSamplers;
    std::vector<std::unique_ptr<LabelsEffects>> labelsEffects;
    std::vector<std::unique_ptr<ButtonSoloCC>> buttonSolos;
    std::vector<std::unique_ptr<ReverbAndBoostSlider>> sliders;
    MasterSliders masterSliders;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> boostAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> reverbSnrAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> reverbHHAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> reverbPercsAttachment;
    
    float yPosPlus = 0.15f;
    
    float waveFormxPos = 0.025f;
    float labelSamplerXpos = 0.325f;
    float labelEffectsXpos = 0.68f;
    float buttonSoloXpos = 0.425f;
    float comboBoxXpos = 0.4625f;
    float reverbSliderXpos = 0.68f;
    float masterSlidersXpos = 0.74f;
    
    ButtonPlayCC buttonPlayCC;
    ButtonStopCC buttonStopCC;
    
    MidiKeyboard keyboardComponent;
    
    LoginComponent loginComponent;
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Twisted_pluginAudioProcessorEditor)
};
