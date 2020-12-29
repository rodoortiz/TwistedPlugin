/*
  ==============================================================================

    ReverbSlider.h
    Created: 4 Dec 2020 10:21:58am
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SlidersLookandFeel.h"

class ReverbAndBoostSlider : public Component {
public:
    ReverbAndBoostSlider(Twisted_pluginAudioProcessor&);
    ~ReverbAndBoostSlider();
    
    static std::vector<std::unique_ptr<ReverbAndBoostSlider>> createObjects(Twisted_pluginAudioProcessor&);
    
    void resized() override;
    
    std::unique_ptr<Slider> slider;
    
private:
    Twisted_pluginAudioProcessor& processor;
    EffectsSlidersLookAndFeel customSlidersLookAndFeel;
    
    //std::unique_ptr<Slider> reverbSlider;
    
    
};
