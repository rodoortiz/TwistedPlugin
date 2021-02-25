/*
  ==============================================================================

    MasterSliders.h
    Created: 13 Dec 2020 7:12:38pm
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SlidersLookandFeel.h"

class MasterSliders : public Component {
public:
    MasterSliders(Twisted_pluginAudioProcessor& p) : processor(p) {
        gainSlider = std::make_unique<Slider>();
        filterSlider = std::make_unique<Slider>();
        lowEQSlider = std::make_unique<Slider>();
        highEQSlider = std::make_unique<Slider>();
        
        gainLabel = std::make_unique<Label>();
        filterLabel = std::make_unique<Label>();
        lowEQLabel = std::make_unique<Label>();
        highEQLabel = std::make_unique<Label>();
        
        //gainSlider attributes
        gainSlider->setLookAndFeel(&customSlidersLookAndFeel);
        gainSlider->setSliderStyle(Slider::RotaryVerticalDrag);
        gainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        gainSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        gainSlider->setRange(-8.0, 2.0, 0.1);
        gainSlider->setValue(-5.0);
        gainSlider->setTextValueSuffix(" dB");
        addAndMakeVisible(gainSlider.get());
        //filterSlider attributes
        filterSlider->setLookAndFeel(&customSlidersLookAndFeel);
        filterSlider->setSliderStyle(Slider::RotaryVerticalDrag);
        filterSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        filterSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        filterSlider->setRange(600.0, 10000.0, 50.0);
        filterSlider->setValue(600.0);
        filterSlider->setTextValueSuffix(" Hz");
        addAndMakeVisible(filterSlider.get());
        //lowEQSlider attributes
        lowEQSlider->setLookAndFeel(&customSlidersLookAndFeel);
        lowEQSlider->setSliderStyle(Slider::RotaryVerticalDrag);
        lowEQSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        lowEQSlider->setColour(Slider::thumbColourId, Colours::aquamarine);
        lowEQSlider->setRange(50.0, 1000.0, 10.0);
        lowEQSlider->setValue(50.0);
        lowEQSlider->setTextValueSuffix(" Hz");
        addAndMakeVisible(lowEQSlider.get());
        //highEQSlider attributes
        highEQSlider->setLookAndFeel(&customSlidersLookAndFeel);
        highEQSlider->setSliderStyle(Slider::RotaryVerticalDrag);
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
        
        gainLabel->setText("VOLUME", dontSendNotification);
        gainLabel->setJustificationType(Justification::centred);
        gainLabel->setFont(Font(16.0f, Font::FontStyleFlags::bold));
        addAndMakeVisible(gainLabel.get());

        filterLabel->setText("FILTER", dontSendNotification);
        filterLabel->setJustificationType(Justification::centred);
        filterLabel->setFont(Font(16.0f, Font::FontStyleFlags::bold));
        addAndMakeVisible(filterLabel.get());

        lowEQLabel->setText("LOW EQ", dontSendNotification);
        lowEQLabel->setJustificationType(Justification::centred);
        lowEQLabel->setFont(Font(16.0f, Font::FontStyleFlags::bold));
        addAndMakeVisible(lowEQLabel.get());

        highEQLabel->setText("HIGH EQ", dontSendNotification);
        highEQLabel->setJustificationType(Justification::centred);
        highEQLabel->setFont(Font(16.0f, Font::FontStyleFlags::bold));
        addAndMakeVisible(highEQLabel.get());
        
    }
    
    ~MasterSliders() override {
        gainSlider->setLookAndFeel(0);
        filterSlider->setLookAndFeel(0);
        lowEQSlider->setLookAndFeel(0);
        highEQSlider->setLookAndFeel(0);
    }
    
    void resized() override {
        gainSlider->setBoundsRelative(0.0f, 0.0f, 1.0f, 0.28f);
        gainLabel->setBoundsRelative(0.0f, 0.28f, 1.0f, 0.05f);

        filterSlider->setBoundsRelative(0.0f, 0.33f, 1.0f, 0.28f);
        filterLabel->setBoundsRelative(0.0f, 0.61f, 1.0f, 0.05f);

        lowEQSlider->setBoundsRelative(0.0f, 0.66f, 0.5f, 0.28f);
        lowEQLabel->setBoundsRelative(0.0f, 0.94f, 0.5f, 0.05f);

        highEQSlider->setBoundsRelative(0.5f, 0.66f, 0.5f, 0.28f);
        highEQLabel->setBoundsRelative(0.5f, 0.94f, 0.5f, 0.05f);
    }
    
    std::unique_ptr<Slider> gainSlider, filterSlider, lowEQSlider, highEQSlider;
private:
    MasterSlidersLookAndFeel customSlidersLookAndFeel;
    
    Twisted_pluginAudioProcessor& processor;
    
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment, filterSliderAttachment, lowEQSliderAttachment, highEQSliderAttachment;
    std::unique_ptr<Label> gainLabel, filterLabel, lowEQLabel, highEQLabel;
};

