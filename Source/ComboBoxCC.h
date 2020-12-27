
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ComboBoxLookAndFeel.h"
#include "FileManager.cpp"

//==============================================================================
/*
*/
class ComboBoxCC  : public juce::Component
{
public:
    ComboBoxCC(Twisted_pluginAudioProcessor& p);
    ~ComboBoxCC() override;
    
    static std::vector<std::unique_ptr<ComboBoxCC>> createObjects(Twisted_pluginAudioProcessor& p);

    void paint (juce::Graphics&) override;
    void resized() override;
    
    Array<File> files;
    ComboBox varComboBox;
    
    int drumSamplerType=0;
    void setDrumSamplerType(int varInt);
    void setDrumSamplerPath (File file)  { drumSamplerPath(file); }

private:
    void drumSamplerPath(File file);
        
    ComboBoxLookAndFeel comboBoxLookAndFeel;
    TextButton textButtonL { "<" };
    TextButton textButtonR { ">" };
    
    Twisted_pluginAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComboBoxCC)
};
