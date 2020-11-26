/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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
    
    void loadFileDragged(const String& path, int& varInt);
    void loadSampleComboBox(const File& file, int& varInt);
    Synthesiser* mySynthSelected(int& varInt);
    AudioBuffer<float>* myAudioBuffer(int& varInt);
    void mySampleNameSelected(int& varInt, String& varString);
    void myAudioLoadedBy(int& varInt, int varInt2);
    void mySoloUpdate(int& varInt, bool varBool);
    void myPlay(bool varBool);
    int myGetNumSamplerSounds(int& varInt);
    int myGetAudioLoadedBy(int& varInt);
    void mySetAudioLoadedBy(int& varInt, int varInt2);
    SamplerSound* mySamplerSound(int& varInt);
    BigInteger myRange(int& varInt);
    int myNote(int& varInt);
    
    AudioBuffer<float> varAudioBuffer0, varAudioBuffer1, varAudioBuffer2, varAudioBuffer3, varAudioBuffer4;
    int audio0LoadedBy = 0, audio1LoadedBy = 0, audio2LoadedBy = 0, audio3LoadedBy = 0, audio4LoadedBy = 0;
    bool solo0, solo1=true, solo2=true, solo3=true, solo4=true, allSolosDisabled=true;
    bool playButtonStarts, playButtonStops;
    MidiMessage messageNote1, messageNote2, messageNote3, messageNote4;
    std::atomic<int> mSampleCount { 0 };
    std::atomic<bool> isNotePlayed { false };
    
    //MidiKeyboard
    MidiKeyboardState keyboardState;
    
    SamplerSound* samplerSound1 {nullptr};
    SamplerSound* samplerSound2 {nullptr};
    SamplerSound* samplerSound3 {nullptr};
    SamplerSound* samplerSound4 {nullptr};
private:
    int currentSynth = 0;
    BigInteger range1, range2, range3, range4;
    int C5note=84, D5note=86, E5note=88, F5note=89;
    
    Synthesiser varSynthesiser0, varSynthesiser1, varSynthesiser2, varSynthesiser3, varSynthesiser4;
    const int numVoices {3};
    
        
    AudioFormatManager varFormatManager;
    AudioFormatReader* varFormatReader {nullptr};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Twisted_pluginAudioProcessor)
};
