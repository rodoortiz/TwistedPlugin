/*
  ==============================================================================

    ButtonSoloCC.h
    Created: 19 Nov 2020 4:42:59pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/*
*/
class ButtonSoloCC  : public juce::Component
{
public:
    ButtonSoloCC(Twisted_pluginAudioProcessor& p);
    ~ButtonSoloCC() override;
    
    static std::vector<std::unique_ptr<ButtonSoloCC>> createObjects(Twisted_pluginAudioProcessor& p);

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setDrumSamplerType(int varInt);
    void aSampleIsLoaded();
    
private:
    int drumSamplerType=0;
    TextButton varTextButton { "" };
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonSoloCC)
};
