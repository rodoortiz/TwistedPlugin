

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
    ButtonLookAndFeel buttonLookAndFeel;
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonPlayCC)
};
