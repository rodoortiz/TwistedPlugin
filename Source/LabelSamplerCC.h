/*
  ==============================================================================

    LabelSamplerCC.h
    Created: 21 Nov 2020 12:41:40pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class LabelSamplerCC  : public juce::Component
{
public:
    LabelSamplerCC(Twisted_pluginAudioProcessor& p);
    ~LabelSamplerCC() override;
    
    static std::vector<std::unique_ptr<LabelSamplerCC>> createObjects(Twisted_pluginAudioProcessor& p);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setDrumSampleLabelText (String string)  { drumSampleLabel.setText(string, dontSendNotification); }
    
    Label drumSampleLabel;
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelSamplerCC)
};

//==============================================================================

class LabelsEffects : public juce::Component
{
public:
    LabelsEffects(Twisted_pluginAudioProcessor& p);
    ~LabelsEffects();

    static std::vector<std::unique_ptr<LabelsEffects>> createObjects(Twisted_pluginAudioProcessor& p);

    void resized() override;

private:
    void setLabelsEffects (String string)  { effectLabel.setText(string, dontSendNotification); }

    Label effectLabel;

    Twisted_pluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelsEffects)
};
