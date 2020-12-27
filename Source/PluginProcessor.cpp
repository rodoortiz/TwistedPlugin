/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Twisted_pluginAudioProcessor::Twisted_pluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", paramLayout()), outputEffects(apvts)
#endif
{
    varFormatManager.registerBasicFormats();
    
    varSynthesiser1.addVoice(new CustomSamplerVoice());
    varSynthesiser2.addVoice(new CustomSamplerVoice());
    varSynthesiser3.addVoice(new CustomSamplerVoice());
    varSynthesiser4.addVoice(new CustomSamplerVoice());
    
    //ranges initializer
    range1.setBit(C5note);
    range1.setBit(C5note-12);
    range2.setBit(D5note);
    range2.setBit(D5note-12);
    range3.setBit(E5note);
    range3.setBit(E5note-12);
    range4.setBit(F5note);
    range4.setBit(F5note-12);
    
    //Get Voice of each synthesiser, each one has one voice
    voiceSynth1 = static_cast<CustomSamplerVoice*>(varSynthesiser1.getVoice(0));
    voiceSynth2 = static_cast<CustomSamplerVoice*>(varSynthesiser2.getVoice(0));
    voiceSynth3 = static_cast<CustomSamplerVoice*>(varSynthesiser3.getVoice(0));
    voiceSynth4 = static_cast<CustomSamplerVoice*>(varSynthesiser4.getVoice(0));
}

Twisted_pluginAudioProcessor::~Twisted_pluginAudioProcessor()
{
    varFormatReader = nullptr;
}

//Creating parameters layout
AudioProcessorValueTreeState::ParameterLayout Twisted_pluginAudioProcessor::paramLayout() {
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>("BOOST", "boost", NormalisableRange<float>(49.0f, 160.0f, 1.0f), 49.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("REVERB_SNR", "reverbSnr", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("REVERB_HH", "reverbHh", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("REVERB_PERCS", "reverbPercs", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("LOW_EQ", "lowEQ", NormalisableRange<float>(49.0f, 1000.0f, 1.0f), 49.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("HIGH_EQ", "highEQ", NormalisableRange<float>(1999.0f, 10000.0f, 1.0f), 1999.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("FILTER", "filter", NormalisableRange<float>(550.0f, 10000.0f, 50.0f), 500.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("GAIN", "gain", NormalisableRange<float>(-8.0f, 2.0f, 0.01f), -5.0f));
    
    return {params.begin(), params.end()};
}

//==============================================================================
const juce::String Twisted_pluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Twisted_pluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Twisted_pluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Twisted_pluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Twisted_pluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Twisted_pluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Twisted_pluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Twisted_pluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Twisted_pluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void Twisted_pluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Twisted_pluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    varSynthesiser1.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser2.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser3.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser4.setCurrentPlaybackSampleRate(sampleRate);
    
    //Set Auxiliar Buffers from Synth Voices
    voiceSynth1->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    voiceSynth2->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    voiceSynth3->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    voiceSynth4->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    
    //DSP prepare
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    outputEffects.prepareDSP(spec);
    reverbSnr.prepareSynthDSP(spec);
    reverbHH.prepareSynthDSP(spec);
    reverbPercs.prepareSynthDSP(spec);
    bassBoost.prepareSynthDSP(spec);
}

void Twisted_pluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Twisted_pluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Twisted_pluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    };
    
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    
    //MIDI MESSAGES
    for (const auto meta : midiMessages)
    {
        const auto msg = meta.getMessage();
        if (msg.isNoteOn())
        {
            switch(msg.getNoteNumber())
            {
                case 84:
                case 72:
                    isNotePlayed1 = true;
                    break;
                case 86:
                case 74:
                    isNotePlayed2 = true;
                    break;
                case 88:
                case 76:
                    isNotePlayed3 = true;
                    break;
                case 89:
                case 77:
                    isNotePlayed4 = true;
                    break;
            }
        }
        else if (msg.isNoteOff())
        {
            switch(msg.getNoteNumber())
            {
                case 84:
                case 72:
                    isNotePlayed1 = false;
                    break;
                case 86:
                case 74:
                    isNotePlayed2 = false;
                    break;
                case 88:
                case 76:
                    isNotePlayed3 = false;
                    break;
                case 89:
                case 77:
                    isNotePlayed4 = false;
                    break;
            }
            if(!isNotePlayed1 && !isNotePlayed2 && !isNotePlayed3 && !isNotePlayed4)
                resetPlayandStopButtons=true;
        }
    }
    
    //PLAYBUTTON
    if(playButtonStarts)
    {
        varSynthesiser1.noteOn(1, C5note, static_cast<uint8>(1));
        varSynthesiser2.noteOn(1, D5note, static_cast<uint8>(1));
        varSynthesiser3.noteOn(1, E5note, static_cast<uint8>(1));
        varSynthesiser4.noteOn(1, F5note, static_cast<uint8>(1));
        isNotePlayed1 = true;
        isNotePlayed2 = true;
        isNotePlayed3 = true;
        isNotePlayed4 = true;
        playButtonStarts=false;
    }
    if(stopButtonStarts)
    {
        varSynthesiser1.noteOff(1, C5note, 0, false);
        varSynthesiser2.noteOff(1, D5note, 0, false);
        varSynthesiser3.noteOff(1, E5note, 0, false);
        varSynthesiser4.noteOff(1, F5note, 0, false);
        isNotePlayed1 = false;
        isNotePlayed2 = false;
        isNotePlayed3 = false;
        isNotePlayed4 = false;
        stopButtonStarts=false;
    }
    
    //PLAYHEAD
    if (isNotePlayed1)
            playheadValue1 = voiceSynth1->getSourceSamplePos();
        else
            playheadValue1=0;
    if (isNotePlayed2)
        playheadValue2 = voiceSynth2->getSourceSamplePos();
        else
            playheadValue2=0;
    if (isNotePlayed3)
        playheadValue3 = voiceSynth3->getSourceSamplePos();
        else
            playheadValue3=0;
    if (isNotePlayed4)
        playheadValue4 = voiceSynth4->getSourceSamplePos();
        else
            playheadValue4=0;
    
    //PLAY SYNTHESISERS
    varSynthesiser1.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    auto sliderValueBoost = apvts.getRawParameterValue("BOOST")->load();
    bassBoost.processSynthBoost(voiceSynth1->getAuxBuffer(), sliderValueBoost); //Get auxiliar buffer from Voice to add effect

    varSynthesiser2.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    auto sliderValueSnr = apvts.getRawParameterValue("REVERB_SNR")->load();
    reverbSnr.processSynthReverb(voiceSynth2->getAuxBuffer(), sliderValueSnr); //Get auxiliar buffer from Voice to add effect

    varSynthesiser3.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    auto sliderValueHH = apvts.getRawParameterValue("REVERB_HH")->load();
    reverbHH.processSynthReverb(voiceSynth3->getAuxBuffer(), sliderValueHH); //Get auxiliar buffer from Voice to add effect

    varSynthesiser4.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    auto sliderValuePercs = apvts.getRawParameterValue("REVERB_PERCS")->load();
    reverbPercs.processSynthReverb(voiceSynth4->getAuxBuffer(), sliderValuePercs); //Get auxiliar buffer from Voice to add effect

    
    //Adding Auxiliar Buffers with effects to Buffer
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {
        
        buffer.addFrom(channel, 0, voiceSynth1->getAuxBuffer(), channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, voiceSynth2->getAuxBuffer(), channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, voiceSynth3->getAuxBuffer(), channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, voiceSynth4->getAuxBuffer(), channel, 0, buffer.getNumSamples());
    }
    
    outputEffects.processDSP(buffer); //Applying master effects
            
    midiMessages.clear(); //Disable plugin to generate MIDI at its output
}

//==============================================================================
bool Twisted_pluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Twisted_pluginAudioProcessor::createEditor()
{
    return new Twisted_pluginAudioProcessorEditor (*this);
}

//==============================================================================
void Twisted_pluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Twisted_pluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//PRIVATE CUSTOM FUNCTIONS
//Read the file
void Twisted_pluginAudioProcessor::loadAFile(const File& file, int& varInt, int varSource)
{
    myAudioLoadedBy(varInt, varSource);
    mySynthSelected(varInt)->clearSounds();
    
    varFormatReader = varFormatManager.createReaderFor(file);
    auto sampleLenght = static_cast<int>(varFormatReader->lengthInSamples);
    myAudioBuffer(varInt)->setSize(1, sampleLenght);
    varFormatReader->read (myAudioBuffer(varInt),0,sampleLenght, 0, true, false);
    
    mySoundToSynth(varInt);
    stopButtonStarts=true;
    resetPlayandStopButtons=true;
}

//Identify the source of the sample (1-drag, 2-combobox) according to the last element used on GUI.
void Twisted_pluginAudioProcessor::myAudioLoadedBy(int& varInt, int varInt2)
{
    switch(varInt)
    {
        case 1:
            audio1LoadedBy = varInt2;
            break;
        case 2:
            audio2LoadedBy = varInt2;
            break;
        case 3:
            audio3LoadedBy = varInt2;
            break;
        case 4:
            audio4LoadedBy = varInt2;
            break;
        default:
            audio1LoadedBy = varInt2;
            break;
    }
}

//Select the synth in accordance with the last element used on GUI
Synthesiser* Twisted_pluginAudioProcessor::mySynthSelected(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return &varSynthesiser1;
        case 2:
            return &varSynthesiser2;
        case 3:
            return &varSynthesiser3;
        case 4:
            return &varSynthesiser4;
        default:
            return &varSynthesiser1;
    }
}

//Select the audioBuffer in accord with the last element used on GUI
AudioBuffer<float>* Twisted_pluginAudioProcessor::myAudioBuffer(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return &varAudioBuffer1;
        case 2:
            return &varAudioBuffer2;
        case 3:
            return &varAudioBuffer3;
        case 4:
            return &varAudioBuffer4;
        default:
            return &varAudioBuffer1;
    }
}

//Assign the customSamplerSound to the corresponding Synthesiser
void Twisted_pluginAudioProcessor::mySoundToSynth(int& varInt)
{
    String nameString = "Sample" + std::to_string(varInt);
    switch(varInt)
    {
        case 1:
            customSamplerSound1 = new CustomSamplerSound(nameString, *varFormatReader, range1, C5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound1);
            break;
        case 2:
            customSamplerSound2 = new CustomSamplerSound(nameString, *varFormatReader, range2, D5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound2);
            break;
        case 3:
            customSamplerSound3 = new CustomSamplerSound(nameString, *varFormatReader, range3, E5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound3);
            break;
        case 4:
            customSamplerSound4 = new CustomSamplerSound(nameString, *varFormatReader, range4, F5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound4);
            break;
    }
}

//Controls the synths thath have perrmission to be listened according to the solo buttons (turned off/turned on)
void Twisted_pluginAudioProcessor::mySoloUpdate(int& varInt, bool varBool)
{
    if(allSolosDisabled)
    {
        playAllowed1=false;
        playAllowed2=false;
        playAllowed3=false;
        playAllowed4=false;
        allSolosDisabled=false;
    }
    
    switch(varInt)
    {
        case 1:
            playAllowed1 = varBool;
            break;
        case 2:
            playAllowed2 = varBool;
            break;
        case 3:
            playAllowed3 = varBool;
            break;
        case 4:
            playAllowed4 = varBool;
            break;
    }
    
    if(!playAllowed1&&!playAllowed2&&!playAllowed3&&!playAllowed4)
    {
        playAllowed1=true;
        playAllowed2=true;
        playAllowed3=true;
        playAllowed4=true;
        allSolosDisabled=true;
    }
    
    if(playAllowed1) voiceSynth1->setVelocity(defaultGain);
    else voiceSynth1->setVelocity(0);
    if(playAllowed2) voiceSynth2->setVelocity(defaultGain);
    else voiceSynth2->setVelocity(0);
    if(playAllowed3) voiceSynth3->setVelocity(defaultGain);
    else voiceSynth3->setVelocity(0);
    if(playAllowed4) voiceSynth4->setVelocity(defaultGain);
    else voiceSynth4->setVelocity(0);
}

//Informs if the synth is allowed to be played
bool Twisted_pluginAudioProcessor::myPlayheadAllowed(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return varSynthesiser1.getVoice(0)->isKeyDown();
        case 2:
            return varSynthesiser2.getVoice(0)->isKeyDown();
        case 3:
            return varSynthesiser3.getVoice(0)->isKeyDown();
        case 4:
            return varSynthesiser4.getVoice(0)->isKeyDown();
    }
    return false;
}

//Buttons Play & Stop
void Twisted_pluginAudioProcessor::myPlay()
{
    playButtonStarts=true;
}
void Twisted_pluginAudioProcessor::myStop()
{
    stopButtonStarts=true;
}

//Informs if an sample in the selected synth has been loaded
int Twisted_pluginAudioProcessor::myGetNumSamplerSounds(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return varSynthesiser1.getNumSounds();
        case 2:
            return varSynthesiser2.getNumSounds();
        case 3:
            return varSynthesiser3.getNumSounds();
        case 4:
            return varSynthesiser4.getNumSounds();
        default:
            return varSynthesiser1.getNumSounds();
    }
    
}

//Gets data of AudioLoadedBy variable, which indicates the sample source
int Twisted_pluginAudioProcessor::myGetAudioLoadedBy(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return audio1LoadedBy;
        case 2:
            return audio2LoadedBy;
        case 3:
            return audio3LoadedBy;
        case 4:
            return audio4LoadedBy;
        default:
            return audio1LoadedBy;
    }
}

//Sets data of AudioLoadedBy variable, which indicates the sample source
void Twisted_pluginAudioProcessor::mySetAudioLoadedBy(int& varInt, int varInt2)
{
    switch(varInt)
    {
        case 1:
            audio1LoadedBy = varInt2;
            break;
        case 2:
            audio2LoadedBy = varInt2;
            break;
        case 3:
            audio3LoadedBy = varInt2;
            break;
        case 4:
            audio4LoadedBy = varInt2;
            break;
        default:
            audio1LoadedBy = varInt2;
            break;
    }
}

//Gets playhead value in accordance with the corresponding synth
std::atomic<int>& Twisted_pluginAudioProcessor::myGetPlayheadValue(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return playheadValue1;
        case 2:
            return playheadValue2;
        case 3:
            return playheadValue3;
        case 4:
            return playheadValue4;
        default:
            return playheadValue1;
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Twisted_pluginAudioProcessor();
}
