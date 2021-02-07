/*
  ==============================================================================

    ButtonStopCC.h
    Created: 30 Nov 2020 7:02:05pm
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
class ButtonStopCC  : public juce::Component
{
public:
    ButtonStopCC(Twisted_pluginAudioProcessor& p);
    ~ButtonStopCC() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    TextButton varTextButton { "" };
    
private:
    ButtonLookAndFeelStop buttonLookAndFeelStop;
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonStopCC)
};
