/*
  ==============================================================================

    ButtonPlayCC.h
    Created: 20 Nov 2020 6:29:19pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ButtonLookAndFeel.h"
//==============================================================================
/*
*/
class ButtonPlayCC  : public juce::Component
{
public:
    ButtonPlayCC(Twisted_pluginAudioProcessor& p);
    ~ButtonPlayCC() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    TextButton varTextButton { "" };

private:
    ButtonLookAndFeelStart buttonLookAndFeelStart;
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonPlayCC)
};
