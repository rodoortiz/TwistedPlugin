
#pragma once
#include <JuceHeader.h>

class MasterSliders : public Component {
public:
    MasterSliders(Twisted_pluginAudioProcessor& p) : processor(p) {
        gainSlider = std::make_unique<Slider>();
        filterSlider = std::make_unique<Slider>();
        lowEQSlider = std::make_unique<Slider>();
        highEQSlider = std::make_unique<Slider>();
        
        //gainSlider attributes
        gainSlider->setSliderStyle(Slider::Rotary);
        gainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        gainSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        gainSlider->setRange(-8.0, 2.0, 0.1);
        gainSlider->setValue(-5.0);
        gainSlider->setTextValueSuffix(" dB");
        addAndMakeVisible(gainSlider.get());
        //filterSlider attributes
        filterSlider->setSliderStyle(Slider::Rotary);
        filterSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        filterSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        filterSlider->setRange(600.0, 10000.0, 50.0);
        filterSlider->setValue(600.0);
        filterSlider->setTextValueSuffix(" Hz");
        addAndMakeVisible(filterSlider.get());
        //lowEQSlider attributes
        lowEQSlider->setSliderStyle(Slider::Rotary);
        lowEQSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        lowEQSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        lowEQSlider->setRange(50.0, 1000.0, 10.0);
        lowEQSlider->setValue(50.0);
        lowEQSlider->setTextValueSuffix(" Hz");
        addAndMakeVisible(lowEQSlider.get());
        //highEQSlider attributes
        highEQSlider->setSliderStyle(Slider::Rotary);
        highEQSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        highEQSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        highEQSlider->setRange(2000.0, 10000.0, 10.0);
        highEQSlider->setValue(2000.0);
        highEQSlider->setTextValueSuffix(" Hz");
        addAndMakeVisible(highEQSlider.get());
        
        gainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "GAIN", *gainSlider);
        filterSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "FILTER", *filterSlider);
        lowEQSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "LOW_EQ", *lowEQSlider);
        highEQSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "HIGH_EQ", *highEQSlider);
        
    }
    
    ~MasterSliders() override {
    }
    
    void resized() override {
        gainSlider->setBoundsRelative(0.0f, 0.0f, 1.0f, 0.33f);
        filterSlider->setBoundsRelative(0.0f, 0.33f, 1.0f, 0.33f);
        lowEQSlider->setBoundsRelative(0.0f, 0.66f, 0.5f, 0.33f);
        highEQSlider->setBoundsRelative(0.5f, 0.66f, 0.5f, 0.33f);
    }
    
private:
    Twisted_pluginAudioProcessor& processor;
    
    std::unique_ptr<Slider> gainSlider, filterSlider, lowEQSlider, highEQSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment, filterSliderAttachment, lowEQSliderAttachment, highEQSliderAttachment;
};

