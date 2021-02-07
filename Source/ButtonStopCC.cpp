/*
  ==============================================================================

    ButtonStopCC.cpp
    Created: 30 Nov 2020 7:02:05pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ButtonStopCC.h"
//==============================================================================
ButtonStopCC::ButtonStopCC(Twisted_pluginAudioProcessor& p) : audioProcessor(p)
{
    varTextButton.setColour(TextButton::buttonColourId, Colours::grey);
    varTextButton.setColour(TextButton::buttonOnColourId, Colours::cadetblue);
    varTextButton.setLookAndFeel(&buttonLookAndFeelStop);
    varTextButton.setButtonText("g");
    varTextButton.setColour (TextButton::textColourOffId, Colours::black);
    varTextButton.setColour (TextButton::textColourOnId, Colours::yellow);
    varTextButton.setToggleState(false, dontSendNotification);
    varTextButton.setEnabled(false);
    addAndMakeVisible(varTextButton);
}

ButtonStopCC::~ButtonStopCC()
{
    varTextButton.setLookAndFeel(0);
}

void ButtonStopCC::paint (juce::Graphics& g)
{

}

void ButtonStopCC::resized()
{
    varTextButton.setBoundsRelative(0, 0, 1, 1);
}
