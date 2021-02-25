/*
  ==============================================================================

    DspProcessor.h
    Created: 21 Dec 2020 10:49:17am
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MasterOutputProcessor {
public:
    MasterOutputProcessor(AudioProcessorValueTreeState&);
    ~MasterOutputProcessor();
    
    void prepareDSP(const dsp::ProcessSpec& spec);
    void processDSP(AudioBuffer<float>& buffer);
    void updateDspChain();
    
private:
    double sampleRate = 0;
    
    using lowEQ = dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>;
    using highEQ = dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>;
    using filter = dsp::StateVariableTPTFilter<float>;
    using gain = dsp::Gain<float>;
    dsp::ProcessorChain<lowEQ, highEQ, filter, gain> dspChain;
    
    AudioProcessorValueTreeState& parameters;
};

//==============================================================================

class SynthProcessor {
public:
    void prepareSynthDSP(const dsp::ProcessSpec& spec);
    void processSynthReverb(AudioBuffer<float>& buffer, float& sliderValue);
    void processSynthBoost(AudioBuffer<float>& buffer, float& sliderValue);
    
private:
    double sampleRate = 0;
    
    dsp::Reverb reverb;
    dsp::Reverb::Parameters reverbParameters;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> bassBoost;
};

