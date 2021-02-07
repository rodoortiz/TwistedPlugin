/*
  ==============================================================================

    LabelSamplerCC.cpp
    Created: 21 Nov 2020 12:41:40pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LabelSamplerCC.h"

//==============================================================================
LabelSamplerCC::LabelSamplerCC(Twisted_pluginAudioProcessor& p) : audioProcessor(p)
{
    drumSampleLabel.setJustificationType(Justification::centred);
   
    addAndMakeVisible(drumSampleLabel);
}

LabelSamplerCC::~LabelSamplerCC()
{
}

std::vector<std::unique_ptr<LabelSamplerCC>> LabelSamplerCC::createObjects(Twisted_pluginAudioProcessor& p)
{
    auto v = std::vector<std::unique_ptr<LabelSamplerCC>>();
    
    for (auto i = 0; i < 4; i++)
    {
        v.emplace_back(std::make_unique<LabelSamplerCC>(p));
        if(i==0)
            v[i]->setDrumSampleLabelText("KICK LOOP");
        if(i==1)
            v[i]->setDrumSampleLabelText("SNARE LOOP");
        if(i==2)
            v[i]->setDrumSampleLabelText("HATS LOOP");
        if(i==3)
            v[i]->setDrumSampleLabelText("PERCUSSION\n LOOP");
    };
    
    return v;
}

void LabelSamplerCC::paint (juce::Graphics& g)
{

}

void LabelSamplerCC::resized()
{
    drumSampleLabel.setBoundsRelative(0, 0, 1, 1);
}
