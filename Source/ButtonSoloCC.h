

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
    
    int drumSamplerType=0;
    void setDrumSamplerType(int varInt);
    void aSampleIsLoaded();
    TextButton varTextButton { "" };

private:
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonSoloCC)
};
