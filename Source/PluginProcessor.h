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

    //LOAD FILES FUNCTIONS
    void loadAFile(const File& file, int& varInt, int comboBoxInt);
    void prepareCleanerCombobox(int& varInt);
    int triggerCleanerCombobox();
    Synthesiser* mySynthSelected(int& varInt);
    AudioBuffer<float>* myAudioBuffer(int& varInt);
    void mySoundToSynth(int& varInt);
    //BUTTONS FUNCTIONS
    void myPlay();
    void myStop();
    void mySoloUpdate(int& varInt, bool varBool);
    void checkIfReset();
    //WAVEFORMS FUNCTIONS
    bool mySamplerSoundExists(int& varInt);
    std::atomic<int>& myGetPlayheadValue(int& varInt);
    String getSampleName(int& varInt);
    //KEYBOARD AND STRETCHING FUNCTIONS
    double getStretchingRatio();
    void checkNoteStatus(int note, std::atomic<bool>& isNotePlayed, RubberBand::RubberBandStretcher& stretcher, CustomSamplerVoice& voice, std::atomic<int>& playHeadValue, double stretcherRatio);

    //REOPEN WINDOW FUNCTIONS
    int getComboBoxInfo(int varInt);
    void setComboBoxInfo(int varInt, int varInt2);
    bool getSoloInfo(int varInt);
    void setSoloInfo(int varInt, bool varBool);
    double getAPVTSInfo(int varInt);
    void setAPVTSInfo(int varInt, double varDouble);


    //MidiKeyboard
    MidiKeyboardState keyboardState;
    //AudioProcessorValueTreeState and listener
    AudioProcessorValueTreeState apvts;
    //DSP
    SynthProcessor reverbSnr, reverbHH, reverbPercs, bassBoost;
    MasterOutputProcessor outputEffects;
    //Let the editor know if it has to reset buttons
    bool buttonsResetProcess;

private:
    //To know if play button is playing
    bool isPlaying;
    //Play allowed indicators
    bool playAllowed1=true, playAllowed2=true, playAllowed3=true, playAllowed4=true;
    //All solos disabled indicator
    bool allSolosDisabled=true;
    //Synthesisers ranges and notes
    int C3note=60, D3note=62, E3note=64, F3note=65;
    BigInteger range1, range2, range3, range4;
    //Default Gains
    float defaultGain = 0.75f;
    //Channels Sliders values
    float sliderValueBoost{0}, sliderValueSnr{0}, sliderValueHH{0}, sliderValuePercs{0};
    //Comboboxes Cleaners
    bool cbcleaner1=false, cbcleaner2=false, cbcleaner3=false, cbcleaner4=false;

    std::atomic<int> playheadValue1 { 0 };
    std::atomic<int> playheadValue2 { 0 };
    std::atomic<int> playheadValue3 { 0 };
    std::atomic<int> playheadValue4 { 0 };
    std::atomic<bool> isNotePlayed1, isNotePlayed2, isNotePlayed3, isNotePlayed4;

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
    AudioBuffer<float> varAudioBuffer1, varAudioBuffer2, varAudioBuffer3, varAudioBuffer4;

    //RUBBERBAND
    std::unique_ptr<RubberBand::RubberBandStretcher> stretcherSynth1, stretcherSynth2, stretcherSynth3, stretcherSynth4;
    AudioBuffer<float> stretchBufferSynth1, stretchBufferSynth2, stretchBufferSynth3, stretchBufferSynth4; //Temporal stretch buffers

    //VALUES TO SAVE WHEN CLOSE SESSION
    //SOLO VALUES
    bool sv1=false, sv2=false, sv3=false, sv4=false;
    //COMBOBOX VALUES
    int cbv1=0, cbv2=0, cbv3=0, cbv4=0;
    //APVTS VALUES
    double knob1=49.0f, knob2=0.0f, knob3=0.0f, knob4=0.0f, knob5=49.0f, knob6=1999.0f, knob7=500.0f, knob8=-5.0f;

    AudioProcessorValueTreeState::ParameterLayout paramLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Twisted_pluginAudioProcessor)
};
