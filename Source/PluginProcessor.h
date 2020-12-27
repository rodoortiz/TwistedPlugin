/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomSynthCC.h"
#include "DspProcessor.h"

//==============================================================================
/**
*/
class Twisted_pluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Twisted_pluginAudioProcessor();
    ~Twisted_pluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void loadAFile(const File& file, int& varInt, int varSource);
    Synthesiser* mySynthSelected(int& varInt);
    AudioBuffer<float>* myAudioBuffer(int& varInt);
    void mySampleNameSelected(int& varInt, String& varString);
    void myAudioLoadedBy(int& varInt, int varInt2);
    void mySoloUpdate(int& varInt, bool varBool);
    bool myPlayheadAllowed(int& varInt);
    void myPlay();
    void myStop();
    int myGetNumSamplerSounds(int& varInt);
    int myGetAudioLoadedBy(int& varInt);
    void mySetAudioLoadedBy(int& varInt, int varInt2);
    void mySoundToSynth(int& varInt);
    std::atomic<int>& myGetPlayheadValue(int& varInt);
    
    AudioBuffer<float> varAudioBuffer1, varAudioBuffer2, varAudioBuffer3, varAudioBuffer4;
    bool resetPlayandStopButtons;

    //MidiKeyboard
    MidiKeyboardState keyboardState;
    //AudioProcessorValueTreeState and listener
    AudioProcessorValueTreeState apvts;
    //DSP
    SynthProcessor reverbSnr, reverbHH, reverbPercs, bassBoost;
    MasterOutputProcessor outputEffects;
    
private:
    //Sample source indicators
    int audio1LoadedBy = 0, audio2LoadedBy = 0, audio3LoadedBy = 0, audio4LoadedBy = 0;
    //Play allowed indicators
    bool playAllowed1=true, playAllowed2=true, playAllowed3=true, playAllowed4=true;
    //All solos disabled indicator
    bool allSolosDisabled=true;
    //Play and Stop controls
    bool playButtonStarts, stopButtonStarts;
    //Synthesisers ranges and notes
    int C5note=84, D5note=86, E5note=88, F5note=89;
    BigInteger range1, range2, range3, range4;
    //Default Gain
    float defaultGain = 0.75f;
    
    std::atomic<int> playheadValue1 { 0 };
    std::atomic<int> playheadValue2 { 0 };
    std::atomic<int> playheadValue3 { 0 };
    std::atomic<int> playheadValue4 { 0 };
    std::atomic<bool> isNotePlayed1 { false };
    std::atomic<bool> isNotePlayed2 { false };
    std::atomic<bool> isNotePlayed3 { false };
    std::atomic<bool> isNotePlayed4 { false };
    
    CustomSamplerSound* customSamplerSound1 {nullptr};
    CustomSamplerSound* customSamplerSound2 {nullptr};
    CustomSamplerSound* customSamplerSound3 {nullptr};
    CustomSamplerSound* customSamplerSound4 {nullptr};
    
    CustomSamplerVoice* voiceSynth1 {nullptr};
    CustomSamplerVoice* voiceSynth2 {nullptr};
    CustomSamplerVoice* voiceSynth3 {nullptr};
    CustomSamplerVoice* voiceSynth4 {nullptr};
    
    Synthesiser varSynthesiser1, varSynthesiser2, varSynthesiser3, varSynthesiser4;
    AudioFormatManager varFormatManager;
    AudioFormatReader* varFormatReader {nullptr};
    
    AudioProcessorValueTreeState::ParameterLayout paramLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Twisted_pluginAudioProcessor)
};
