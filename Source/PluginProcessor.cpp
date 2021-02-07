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
    range1D.setBit(C4note);
    range2.setBit(D5note);
    range2D.setBit(D4note);
    range3.setBit(E5note);
    range3D.setBit(E4note);
    range4.setBit(F5note);
    range4D.setBit(F4note);

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

    //Set Auxiliar Buffers from Synth Voices and Temporal Buffers for sretching
    voiceSynth1->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    voiceSynth2->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    voiceSynth3->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    voiceSynth4->setAuxBuffer(getTotalNumOutputChannels(), samplesPerBlock);
    stretchBufferSynth1.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    stretchBufferSynth2.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    stretchBufferSynth3.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    stretchBufferSynth4.setSize(getTotalNumOutputChannels(), samplesPerBlock);

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

    stretcherSynth1 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1, 1);
    stretcherSynth1->reset();
    stretcherSynth2 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1, 1);
    stretcherSynth2->reset();
    stretcherSynth3 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1, 1);
    stretcherSynth3->reset();
    stretcherSynth4 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1, 1);
    stretcherSynth4->reset();
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

    //PLAY KEYS OF KEYBOARD
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    //OCTAVE5
    if(keyboardState.isNoteOn(1, C5note)){
        if(!isNotePlayed1){
            isNotePlayed1 = true;
            myGetBPM();
            stretcherSynth1->setPitchScale(1);
            stretcherSynth1->setTimeRatio(stretchingRatio);
            voiceSynth1->setSourceSamplePos0();}
        playheadValue1 = voiceSynth1->getSourceSamplePos();}
    else if(isNotePlayed1){
        isNotePlayed1 = false;
        playheadValue1 = 0;
        stretcherSynth1->reset();}

    if(keyboardState.isNoteOn(1, D5note)){
        if(!isNotePlayed2){
            isNotePlayed2 = true;
            myGetBPM();
            stretcherSynth2->setPitchScale(1);
            stretcherSynth2->setTimeRatio(stretchingRatio);
            voiceSynth2->setSourceSamplePos0();}
        playheadValue2 = voiceSynth2->getSourceSamplePos();}
    else if(isNotePlayed2){
        isNotePlayed2 = false;
        playheadValue2 = 0;
        stretcherSynth2->reset();}

    if(keyboardState.isNoteOn(1, E5note)){
        if(!isNotePlayed3){
            isNotePlayed3 = true;
            myGetBPM();
            stretcherSynth3->setPitchScale(1);
            stretcherSynth3->setTimeRatio(stretchingRatio);
            voiceSynth3->setSourceSamplePos0();}
        playheadValue3 = voiceSynth3->getSourceSamplePos();}
    else if(isNotePlayed3){
        isNotePlayed3 = false;
        playheadValue3 = 0;
        stretcherSynth3->reset();}

    if(keyboardState.isNoteOn(1, F5note)){
        if(!isNotePlayed4){
            isNotePlayed4 = true;
            myGetBPM();
            stretcherSynth4->setPitchScale(1);
            stretcherSynth4->setTimeRatio(stretchingRatio);
            voiceSynth4->setSourceSamplePos0();}
        playheadValue4 = voiceSynth4->getSourceSamplePos();}
    else if(isNotePlayed4){
        isNotePlayed4 = false;
        playheadValue4 = 0;
        stretcherSynth4->reset();}

    //OCTAVE4
    if(keyboardState.isNoteOn(1, C4note)){
        if(!isNotePlayed1D){
            isNotePlayed1D = true;
            myGetBPM();
            stretcherSynth1->setPitchScale(0.5);
            stretcherSynth1->setTimeRatio(stretchingRatio);
            voiceSynth1->setSourceSamplePos0();}
        playheadValue1 = voiceSynth1->getSourceSamplePos();}
    else if(isNotePlayed1D){
        isNotePlayed1D = false;
        playheadValue1 = 0;
        stretcherSynth1->reset();}

    if(keyboardState.isNoteOn(1, D4note)){
        if(!isNotePlayed2D){
            isNotePlayed2D = true;
            myGetBPM();
            stretcherSynth2->setPitchScale(0.5);
            stretcherSynth2->setTimeRatio(stretchingRatio);
            voiceSynth2->setSourceSamplePos0();}
        playheadValue2 = voiceSynth2->getSourceSamplePos();}
    else if(isNotePlayed2D){
        isNotePlayed2D = false;
        playheadValue2 = 0;
        stretcherSynth2->reset();}

    if(keyboardState.isNoteOn(1, E4note)){
        if(!isNotePlayed3D){
            isNotePlayed3D = true;
            myGetBPM();
            stretcherSynth3->setPitchScale(0.5);
            stretcherSynth3->setTimeRatio(stretchingRatio);
            voiceSynth3->setSourceSamplePos0();}
        playheadValue3 = voiceSynth3->getSourceSamplePos();}
    else if(isNotePlayed3D){
        isNotePlayed3D = false;
        playheadValue3 = 0;
        stretcherSynth3->reset();}

    if(keyboardState.isNoteOn(1, F4note)){
        if(!isNotePlayed4D){
            isNotePlayed4D = true;
            myGetBPM();
            stretcherSynth4->setPitchScale(0.5);
            stretcherSynth4->setTimeRatio(stretchingRatio);
            voiceSynth4->setSourceSamplePos0();}
        playheadValue4 = voiceSynth4->getSourceSamplePos();}
    else if(isNotePlayed4D){
        isNotePlayed4D = false;
        playheadValue4 = 0;
        stretcherSynth4->reset();}

    //PLAY AND STOP BUTTONS
    if(playButtonStarts)
    {
        keyboardState.noteOn(1, C5note, static_cast<uint8>(1));
        keyboardState.noteOn(1, D5note, static_cast<uint8>(1));
        keyboardState.noteOn(1, E5note, static_cast<uint8>(1));
        keyboardState.noteOn(1, F5note, static_cast<uint8>(1));
        isPlaying=true;
        playButtonStarts=false;
    }
    if(stopButtonStarts)
    {
        isPlaying=false;
        keyboardState.allNotesOff(1);
        stretcherSynth1->reset();
        stretcherSynth2->reset();
        stretcherSynth3->reset();
        stretcherSynth4->reset();
        stopButtonStarts=false;
    }

    //PLAY SYNTHESISERS
    auto outputBufferSamples = buffer.getNumSamples();

    //Synth 1
    auto readPointers = voiceSynth1->getAuxBuffer().getArrayOfReadPointers(); //Read from Aux Buffer of Synth 1
    auto writePointers = stretchBufferSynth1.getArrayOfWritePointers(); //Write to temporal stretch buffer
    auto samplesAvailable = stretcherSynth1->available(); //Available samples for stretcher

    while(samplesAvailable < outputBufferSamples)
    {
        varSynthesiser1.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        stretcherSynth1->process(readPointers, voiceSynth1->getAuxBuffer().getNumSamples(), false);
        samplesAvailable = stretcherSynth1->available();
    }

    stretcherSynth1->retrieve(writePointers, stretchBufferSynth1.getNumSamples()); //Return stretch process to temporal stretch buffer
    sliderValueBoost = apvts.getRawParameterValue("BOOST")->load(); //Get value of slider
    bassBoost.processSynthBoost(stretchBufferSynth1, sliderValueBoost); //Add channel effect to temporal stretch buffer

    //Synth 2
    readPointers = voiceSynth2->getAuxBuffer().getArrayOfReadPointers(); //Read from Aux Buffer of Synth 1
    writePointers = stretchBufferSynth2.getArrayOfWritePointers(); //Write to temporal stretch buffer
    samplesAvailable = stretcherSynth2->available(); //Available samples for stretcher

    while(samplesAvailable < outputBufferSamples)
    {
        varSynthesiser2.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        stretcherSynth2->process(readPointers, voiceSynth2->getAuxBuffer().getNumSamples(), false);
        samplesAvailable = stretcherSynth2->available();
    }

    stretcherSynth2->retrieve(writePointers, stretchBufferSynth2.getNumSamples()); //Return stretch process to temporal stretch buffer
    sliderValueSnr = apvts.getRawParameterValue("REVERB_SNR")->load(); //Get value of slider
    reverbSnr.processSynthReverb(stretchBufferSynth2, sliderValueSnr); //Add channel effect to temporal stretch buffer

    //Synth 3
    readPointers = voiceSynth3->getAuxBuffer().getArrayOfReadPointers(); //Read from Aux Buffer of Synth 1
    writePointers = stretchBufferSynth3.getArrayOfWritePointers(); //Write to temporal stretch buffer
    samplesAvailable = stretcherSynth3->available(); //Available samples for stretcher

    while(samplesAvailable < outputBufferSamples)
    {
        varSynthesiser3.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        stretcherSynth3->process(readPointers, voiceSynth3->getAuxBuffer().getNumSamples(), false);
        samplesAvailable = stretcherSynth3->available();
    }

    stretcherSynth3->retrieve(writePointers, stretchBufferSynth3.getNumSamples()); //Return stretch process to temporal stretch buffer
    sliderValueHH = apvts.getRawParameterValue("REVERB_HH")->load(); //Get value of slider
    reverbHH.processSynthReverb(stretchBufferSynth3, sliderValueHH); //Add channel effect to temporal stretch buffer

    //Synth 4
    readPointers = voiceSynth4->getAuxBuffer().getArrayOfReadPointers(); //Read from Aux Buffer of Synth 1
    writePointers = stretchBufferSynth4.getArrayOfWritePointers(); //Write to temporal stretch buffer
    samplesAvailable = stretcherSynth4->available(); //Available samples for stretcher

    while(samplesAvailable < outputBufferSamples)
    {
        varSynthesiser4.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        stretcherSynth4->process(readPointers, voiceSynth4->getAuxBuffer().getNumSamples(), false);
        samplesAvailable = stretcherSynth4->available();
    }

    stretcherSynth4->retrieve(writePointers, stretchBufferSynth4.getNumSamples()); //Return stretch process to temporal stretch buffer
    sliderValuePercs = apvts.getRawParameterValue("REVERB_PERCS")->load(); //Get value of slider
    reverbPercs.processSynthReverb(stretchBufferSynth4, sliderValuePercs); ; //Add channel effect to temporal stretch buffer

    //Adding Temporal Stretch Buffers with effects to Buffer
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {

        buffer.addFrom(channel, 0, stretchBufferSynth1, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, stretchBufferSynth2, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, stretchBufferSynth3, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, stretchBufferSynth4, channel, 0, buffer.getNumSamples());
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
            customSamplerSound1D = new CustomSamplerSound(nameString, *varFormatReader, range1D, C4note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound1D);
            break;
        case 2:
            customSamplerSound2 = new CustomSamplerSound(nameString, *varFormatReader, range2, D5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound2);
            customSamplerSound2D = new CustomSamplerSound(nameString, *varFormatReader, range2D, D4note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound2D);
            break;
        case 3:
            customSamplerSound3 = new CustomSamplerSound(nameString, *varFormatReader, range3, E5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound3);
            customSamplerSound3D = new CustomSamplerSound(nameString, *varFormatReader, range3D, E4note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound3D);
            break;
        case 4:
            customSamplerSound4 = new CustomSamplerSound(nameString, *varFormatReader, range4, F5note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound4);
            customSamplerSound4D = new CustomSamplerSound(nameString, *varFormatReader, range4D, F4note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound4D);
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

//Buttons Play & Stop
void Twisted_pluginAudioProcessor::myPlay()
{
    playButtonStarts=true;
}
void Twisted_pluginAudioProcessor::myStop()
{
    stopButtonStarts=true;
}

bool Twisted_pluginAudioProcessor::mySamplerSoundExists(int& varInt)
{
    switch(varInt)
    {
        case 1:
            if(customSamplerSound1 != nullptr)
                return true;
            else
                return false;
        case 2:
            if(customSamplerSound2 != nullptr)
                return true;
            else
                return false;
        case 3:
            if(customSamplerSound3 != nullptr)
                return true;
            else
                return false;
        case 4:
            if(customSamplerSound4 != nullptr)
                return true;
            else
                return false;
        default:
            return false;
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

//BPM
void Twisted_pluginAudioProcessor::myGetBPM()
{
    playHead = this->getPlayHead();
    playHead->getCurrentPosition (currentPositionInfo);
    stretchingRatio=130/currentPositionInfo.bpm;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Twisted_pluginAudioProcessor();
}
