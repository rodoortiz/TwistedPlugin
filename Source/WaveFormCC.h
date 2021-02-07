/*
  ==============================================================================

    WaveFormCC.h
    Created: 6 Nov 2020 12:37:55pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveFormCC  : public juce::Component,
                    public FileDragAndDropTarget
{
public:
    WaveFormCC(Twisted_pluginAudioProcessor& p);
    ~WaveFormCC() override;
    
    static std::vector<std::unique_ptr<WaveFormCC>> createObjects(Twisted_pluginAudioProcessor& p);

    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;

    void setDrumSamplerType(int varInt);

private:
    std::vector<float> mAudioPoints;
    int drumSamplerType=0;

    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveFormCC)
};
