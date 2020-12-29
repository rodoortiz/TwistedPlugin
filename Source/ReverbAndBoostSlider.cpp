/*
  ==============================================================================

    ReverbSlider.cpp
    Created: 4 Dec 2020 10:21:58am
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#include "ReverbAndBoostSlider.h"

ReverbAndBoostSlider::ReverbAndBoostSlider(Twisted_pluginAudioProcessor& p) : processor(p) {
    slider = std::make_unique<Slider>();
    
    slider->setLookAndFeel(&customSlidersLookAndFeel);
    slider->setSliderStyle(Slider::Rotary);
    slider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
    slider->setColour(Slider::thumbColourId, Colours::cadetblue);
    slider->setRange(0, 10, 0.1);
    addAndMakeVisible(slider.get());
    
}

ReverbAndBoostSlider::~ReverbAndBoostSlider() {
    
}

std::vector<std::unique_ptr<ReverbAndBoostSlider>> ReverbAndBoostSlider::createObjects(Twisted_pluginAudioProcessor& p) {
     
    auto sliderVector = std::vector<std::unique_ptr<ReverbAndBoostSlider>>();
    
    for (auto i = 0; i < 4; i++) {
        sliderVector.emplace_back(std::make_unique<ReverbAndBoostSlider>(p));
    }
    
    return sliderVector;
}

void ReverbAndBoostSlider::resized() {
    slider->setBoundsRelative(0, 0, 1, 1);
}
