/*
  ==============================================================================
 
    DspProcessor.cpp
    Created: 7 Dec 2020 5:49:47pm
    Author:  Rodolfo Ortiz
 
  ==============================================================================
*/

#include "DspProcessor.h"

MasterOutputProcessor::MasterOutputProcessor(AudioProcessorValueTreeState& apvts) : parameters(apvts) {
}

MasterOutputProcessor::~MasterOutputProcessor() {
}

void MasterOutputProcessor::prepareDSP(const dsp::ProcessSpec& spec) {
    
    dspChain.prepare(spec);
    
    //Initialize Filter as Lowpass
    dspChain.get<2>().setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    sampleRate = spec.sampleRate;
}

void MasterOutputProcessor::processDSP(AudioBuffer<float>& buffer) {
        
    updateDspChain();
    
    auto audioBlock = dsp::AudioBlock<float> (buffer);
    auto context = dsp::ProcessContextReplacing<float> (audioBlock);
    dspChain.process(context);
}

void MasterOutputProcessor::updateDspChain() {
    
    //Update resonance frequency for low boost EQ
    auto lowEQFreq = parameters.getRawParameterValue("LOW_EQ")->load();
    if (lowEQFreq < 50.0f) {
    *dspChain.get<0>().state = *dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, lowEQFreq, 0.1, 1);
    } else {
    *dspChain.get<0>().state = *dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, lowEQFreq, 1.0, 4.0);
    }
    
    //Update resonance frequency for high boost EQ
    auto highEQFreq = parameters.getRawParameterValue("HIGH_EQ")->load();
    if (highEQFreq < 2000.0f) {
    *dspChain.get<1>().state = *dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, highEQFreq, 0.1, 1);
    } else {
    *dspChain.get<1>().state = *dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, highEQFreq, 1.0, 4.0);
    }
    
    //Update cutoff frequency for lowpass filter
    auto filterFreq = parameters.getRawParameterValue("FILTER")->load();
    if (filterFreq < 600.0f) {
        dspChain.get<2>().setCutoffFrequency(20000.f);
    } else {
        dspChain.get<2>().setCutoffFrequency(filterFreq);
    }
    
    //Update gain value
    auto gainValue = parameters.getRawParameterValue("GAIN")->load();
    dspChain.get<3>().setGainDecibels(gainValue);
}

//==============================================================================

//SynthProcessor::SynthProcessor() {
//}
//
//SynthProcessor::~SynthProcessor() {
//}

void SynthProcessor::prepareSynthDSP(const dsp::ProcessSpec& spec) {
    reverb.prepare(spec);
    bassBoost.prepare(spec);
    sampleRate = spec.sampleRate;
}

void SynthProcessor::processSynthReverb(AudioBuffer<float> &buffer, float& sliderValue) {
    
    reverbParameters.dryLevel = 1 - sliderValue;
    reverbParameters.wetLevel = sliderValue;
    reverb.setParameters(reverbParameters);
    
    auto audioBlock = dsp::AudioBlock<float> (buffer);
    auto context = dsp::ProcessContextReplacing<float> (audioBlock);
    reverb.process(context);
}

void SynthProcessor::processSynthBoost(AudioBuffer<float>& buffer, float& sliderValue) {
    
    if (sliderValue < 50.0f) {
    *bassBoost.state = *dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, sliderValue, 0.1, 1);
    } else {
    *bassBoost.state = *dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, sliderValue, 1.0, 3.5);
    }
    
    auto audioBlock = dsp::AudioBlock<float> (buffer);
    auto context = dsp::ProcessContextReplacing<float> (audioBlock);
    bassBoost.process(context);

}
