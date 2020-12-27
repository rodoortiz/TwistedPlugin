
#include <JuceHeader.h>
#include "ButtonSoloCC.h"

//==============================================================================
ButtonSoloCC::ButtonSoloCC(Twisted_pluginAudioProcessor& p) : audioProcessor(p)
{
    varTextButton.onClick = [&](){
        if(varTextButton.getToggleState())
            varTextButton.setToggleState(false, dontSendNotification);
        else
            varTextButton.setToggleState(true, dontSendNotification);
        audioProcessor.mySoloUpdate(drumSamplerType, varTextButton.getToggleState());
    };
    
    varTextButton.setColour(TextButton::buttonColourId, Colours::grey);
    varTextButton.setColour(TextButton::buttonOnColourId, Colours::cadetblue);
    varTextButton.setButtonText("S");
    varTextButton.setColour (TextButton::textColourOffId, Colours::black);
    varTextButton.setColour (TextButton::textColourOnId, Colours::yellow);
    varTextButton.setToggleState(false, dontSendNotification);
    varTextButton.setEnabled(false);
    addAndMakeVisible(varTextButton);
}

ButtonSoloCC::~ButtonSoloCC()
{
}

std::vector<std::unique_ptr<ButtonSoloCC>> ButtonSoloCC::createObjects(Twisted_pluginAudioProcessor& p)
{
    auto v = std::vector<std::unique_ptr<ButtonSoloCC>>();
    
    for (auto i = 0; i < 4; i++)
    {
        v.emplace_back(std::make_unique<ButtonSoloCC>(p));
        v[i]->setDrumSamplerType(i+1);
    };
    
    return v;
}

void ButtonSoloCC::paint (juce::Graphics& g)
{
    if(audioProcessor.myGetNumSamplerSounds(drumSamplerType)>0)
    {
        aSampleIsLoaded();
    }
}

void ButtonSoloCC::resized()
{
    varTextButton.setBoundsRelative(0, 0, 1, 1);
}

//-----
void ButtonSoloCC::setDrumSamplerType(int varInt)
{
    drumSamplerType = varInt;
}

void ButtonSoloCC::aSampleIsLoaded()
{
    varTextButton.setEnabled(true);
}
