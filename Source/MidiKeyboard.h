
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
