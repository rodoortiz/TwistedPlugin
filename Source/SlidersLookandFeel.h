/*
  ==============================================================================

    SlidersLookandFeel.h
    Created: 28 Dec 2020 7:57:11pm
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class EffectsSlidersLookAndFeel : public LookAndFeel_V4 {
public:
    EffectsSlidersLookAndFeel() {
        knobImage = ImageCache::getFromMemory(BinaryData::Knob_example_png, BinaryData::Knob_example_pngSize);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        
        //Rotating knob image equation
        float stopValue = 0.8;
        float rotation = stopValue * MathConstants<float>::twoPi * sliderPos;
        
        knobImage = knobImage.rescaled((int)radius*2, (int)radius*2, Graphics::ResamplingQuality::highResamplingQuality);

        g.drawImageTransformed(knobImage, AffineTransform::rotation(rotation, knobImage.getWidth()/2.0, knobImage.getHeight()/2.0).translated(rx, ry));
    }
private:
    Image knobImage;
};

class MasterSlidersLookAndFeel : public LookAndFeel_V4 {
public:
    MasterSlidersLookAndFeel() {
        knobImage = ImageCache::getFromMemory(BinaryData::Knob_example_png, BinaryData::Knob_example_pngSize);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        
        //Rotating knob image equation
        float stopValue = 0.8;
        float rotation = stopValue * MathConstants<float>::twoPi * sliderPos;
        
        knobImage = knobImage.rescaled((int)radius*2, (int)radius*2, Graphics::ResamplingQuality::highResamplingQuality);

        g.drawImageTransformed(knobImage, AffineTransform::rotation(rotation, knobImage.getWidth()/2.0, knobImage.getHeight()/2.0).translated(rx, ry));
    }
    
private:
    Image knobImage;
};

