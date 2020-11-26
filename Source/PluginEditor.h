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
#include <array>

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
    
private:
    Image imagen;
    ImageComponent imagenComponent;
    
    std::vector<std::unique_ptr<WaveFormCC>> waveForms;
    std::vector<std::unique_ptr<ComboBoxCC>> comboBoxes;
    std::vector<std::unique_ptr<LabelSamplerCC>> labelSamplers;
    std::vector<std::unique_ptr<ButtonSoloCC>> buttonSolos;
    
    float yPosPlus = 0.15f;
    
    float waveFormxPos = 0.025f;
    float labelSamplerXpos = 0.325f;
    float buttonSoloXpos = 0.425f;
    float comboBoxXpos = 0.4625f;

    Twisted_pluginAudioProcessor& audioProcessor;
    
    MidiKeyboard keyboardComponent;
    
    ButtonPlayCC buttonPlayCC;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Twisted_pluginAudioProcessorEditor)
};
