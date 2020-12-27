
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ReverbAndBoostSlider : public Component {
public:
    ReverbAndBoostSlider(Twisted_pluginAudioProcessor&);
    ~ReverbAndBoostSlider();
    
    static std::vector<std::unique_ptr<ReverbAndBoostSlider>> createObjects(Twisted_pluginAudioProcessor&);
    
    void resized() override;
    
    std::unique_ptr<Slider> slider;
    
private:
    Twisted_pluginAudioProcessor& processor;
    
    //std::unique_ptr<Slider> reverbSlider;
    
    
};
