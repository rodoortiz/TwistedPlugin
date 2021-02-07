/*
  ==============================================================================

    MidiKeyboard.h
    Created: 20 Nov 2020 10:16:29am
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MidiKeyboard.h"
#include "PluginProcessor.h"

class MidiKeyboard : public Component {
public:
    MidiKeyboard(Twisted_pluginAudioProcessor&);
    ~MidiKeyboard();
    
    void resized() override;
    
    
private:
    Twisted_pluginAudioProcessor& processor;
    std::unique_ptr<MidiKeyboardComponent> keyboardComponent;
};
