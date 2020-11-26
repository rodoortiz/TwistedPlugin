

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
    
    int drumSamplerType=0;
    void setDrumSamplerType(int varInt);

private:
    AudioBuffer<float> myWaveForm();
        
    std::vector<float> mAudioPoints;
    bool mShouldBePainting{false};

    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveFormCC)
};
