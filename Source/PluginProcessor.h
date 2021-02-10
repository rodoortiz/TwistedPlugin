/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomSynthCC.h"
#include "DspProcessor.h"
#include <rubberband/RubberBandStretcher.h>

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
    void myPlay();
    void myStop();
    bool mySamplerSoundExists(int& varInt);
    int myGetAudioLoadedBy(int& varInt);
    void mySetAudioLoadedBy(int& varInt, int varInt2);
    void mySoundToSynth(int& varInt);
    std::atomic<int>& myGetPlayheadValue(int& varInt);
    void myGetBPM();

    AudioBuffer<float> varAudioBuffer1, varAudioBuffer2, varAudioBuffer3, varAudioBuffer4;
    bool isPlaying, buttonsResetProcess;

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
    int C4note=72, D4note=74, E4note=76, F4note=77;
    BigInteger range1, range2, range3, range4;
    BigInteger range1D, range2D, range3D, range4D;
    //Default Gain
    float defaultGain = 0.75f;
    //Channels Sliders values
    float sliderValueBoost{0}, sliderValueSnr{0}, sliderValueHH{0}, sliderValuePercs{0};

    std::atomic<int> playheadValue1 { 0 };
    std::atomic<int> playheadValue2 { 0 };
    std::atomic<int> playheadValue3 { 0 };
    std::atomic<int> playheadValue4 { 0 };
    bool isNotePlayed1, isNotePlayed2, isNotePlayed3, isNotePlayed4;
    bool isNotePlayed1D, isNotePlayed2D, isNotePlayed3D, isNotePlayed4D;

    CustomSamplerSound* customSamplerSound1 {nullptr};
    CustomSamplerSound* customSamplerSound2 {nullptr};
    CustomSamplerSound* customSamplerSound3 {nullptr};
    CustomSamplerSound* customSamplerSound4 {nullptr};
    CustomSamplerSound* customSamplerSound1D {nullptr};
    CustomSamplerSound* customSamplerSound2D {nullptr};
    CustomSamplerSound* customSamplerSound3D {nullptr};
    CustomSamplerSound* customSamplerSound4D {nullptr};

    CustomSamplerVoice* voiceSynth1 {nullptr};
    CustomSamplerVoice* voiceSynth2 {nullptr};
    CustomSamplerVoice* voiceSynth3 {nullptr};
    CustomSamplerVoice* voiceSynth4 {nullptr};

    Synthesiser varSynthesiser1, varSynthesiser2, varSynthesiser3, varSynthesiser4;
    AudioFormatManager varFormatManager;
    AudioFormatReader* varFormatReader {nullptr};

    //RUBBERBAND
    std::unique_ptr<RubberBand::RubberBandStretcher> stretcherSynth1, stretcherSynth2, stretcherSynth3, stretcherSynth4;
    AudioBuffer<float> stretchBufferSynth1, stretchBufferSynth2, stretchBufferSynth3, stretchBufferSynth4; //Temporal stretch buffers

    //BPM
    AudioPlayHead* playHead;
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    float stretchingRatio;

    AudioProcessorValueTreeState::ParameterLayout paramLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Twisted_pluginAudioProcessor)
};
