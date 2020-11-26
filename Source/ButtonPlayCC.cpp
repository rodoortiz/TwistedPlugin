

#include <JuceHeader.h>
#include "ButtonPlayCC.h"
//==============================================================================
ButtonPlayCC::ButtonPlayCC(Twisted_pluginAudioProcessor& p) : audioProcessor(p)
{
    varTextButton.onClick = [&](){
        if(varTextButton.getToggleState())
            varTextButton.setToggleState(false, dontSendNotification);
        else
            varTextButton.setToggleState(true, dontSendNotification);
        audioProcessor.myPlay(varTextButton.getToggleState());
    };
    
    varTextButton.setColour(TextButton::buttonColourId, Colours::grey);
    varTextButton.setColour(TextButton::buttonOnColourId, Colours::cadetblue);
    varTextButton.setLookAndFeel(&buttonLookAndFeel);
    varTextButton.setButtonText("u");
    varTextButton.setColour (TextButton::textColourOffId, Colours::black);
    varTextButton.setColour (TextButton::textColourOnId, Colours::yellow);
    varTextButton.setToggleState(false, dontSendNotification);
    varTextButton.setEnabled(true);
    addAndMakeVisible(varTextButton);

}

ButtonPlayCC::~ButtonPlayCC()
{
    varTextButton.setLookAndFeel(0);
}

void ButtonPlayCC::paint (juce::Graphics& g)
{

}

void ButtonPlayCC::resized()
{
    varTextButton.setBoundsRelative(0, 0, 1, 1);
}
