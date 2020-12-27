
#include <JuceHeader.h>
#include "MidiKeyboard.h"

MidiKeyboard::MidiKeyboard(Twisted_pluginAudioProcessor& p) : processor(p) {
    keyboardComponent = std::make_unique<MidiKeyboardComponent>(processor.keyboardState, MidiKeyboardComponent::horizontalKeyboard);
    
    keyboardComponent->setLowestVisibleKey(69); //45 = A1
    keyboardComponent->setKeyWidth(35.0f);
    addAndMakeVisible(keyboardComponent.get());
    
}

MidiKeyboard::~MidiKeyboard() {

}

void MidiKeyboard::resized() {
    keyboardComponent->setBoundsRelative(0, 0, 1, 1);
}
