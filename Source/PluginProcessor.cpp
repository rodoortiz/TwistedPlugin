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
                       )
#endif
{
    varFormatManager.registerBasicFormats();
    
    for(int i = 0; i < numVoices; i++)
    {
        varSynthesiser0.addVoice(new SamplerVoice());
        varSynthesiser1.addVoice(new SamplerVoice());
        varSynthesiser2.addVoice(new SamplerVoice());
        varSynthesiser3.addVoice(new SamplerVoice());
        varSynthesiser4.addVoice(new SamplerVoice());
    }
    
    //ranges initializer
    range1.setBit(C5note);
    range1.setBit(C5note-12);
    range2.setBit(D5note);
    range2.setBit(D5note-12);
    range3.setBit(E5note);
    range3.setBit(E5note-12);
    range4.setBit(F5note);
    range4.setBit(F5note-12);
}

Twisted_pluginAudioProcessor::~Twisted_pluginAudioProcessor()
{
    varFormatReader = nullptr;
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
    varSynthesiser0.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser1.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser2.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser3.setCurrentPlaybackSampleRate(sampleRate);
    varSynthesiser4.setCurrentPlaybackSampleRate(sampleRate);
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    
    //MIDI MESSAGES
    for (const auto meta : midiMessages) {
        const auto msg = meta.getMessage();
        if (msg.isNoteOn()){
            isNotePlayed = true;}
        else if (msg.isNoteOff()){
            isNotePlayed = false;
            mSampleCount=0;}
    }
    
    //PLAYBUTTON
    if(playButtonStarts)
    {
        messageNote1 = MidiMessage::noteOn (1, C5note, (uint8) 100);
        midiMessages.addEvent(messageNote1, 0);
        messageNote2 = MidiMessage::noteOn (1, D5note, (uint8) 100);
        midiMessages.addEvent(messageNote2, 0);
        messageNote3 = MidiMessage::noteOn (1, E5note, (uint8) 100);
        midiMessages.addEvent(messageNote3, 0);
        messageNote4 = MidiMessage::noteOn (1, F5note, (uint8) 100);
        midiMessages.addEvent(messageNote4, 0);
        isNotePlayed = true;
        playButtonStarts=false;
    }
    if(playButtonStops)
    {
        messageNote1 = MidiMessage::noteOff (1, C5note, (uint8) 100);
        midiMessages.addEvent(messageNote1, 0);
        messageNote2 = MidiMessage::noteOff (1, D5note, (uint8) 100);
        midiMessages.addEvent(messageNote2, 0);
        messageNote3 = MidiMessage::noteOff (1, E5note, (uint8) 100);
        midiMessages.addEvent(messageNote3, 0);
        messageNote4 = MidiMessage::noteOff (1, F5note, (uint8) 100);
        midiMessages.addEvent(messageNote4, 0);
        isNotePlayed = false;
        playButtonStops=false;
    }
    
    
    //PLAYHEAD
    if (isNotePlayed) {
        //int numUno = 1;
        //SamplerSound* samplerSound = mySamplerSound(numUno);
        //DBG(samplerSound->getName());
        mSampleCount += buffer.getNumSamples();
    } else {
        mSampleCount=0;
    }

    
    
    //PLAY SYNTHESISERS
    if(solo1)
        varSynthesiser1.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
     if(solo2)
         varSynthesiser2.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
     if(solo3)
         varSynthesiser3.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
     if(solo4)
         varSynthesiser4.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    
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



//OWN FUNCTIONS
void Twisted_pluginAudioProcessor::loadFileDragged(const String& path, int& varInt)
{
    myAudioLoadedBy(varInt, 1);
    mySynthSelected(varInt)->clearSounds();
    
    auto file = File(path);
    varFormatReader = varFormatManager.createReaderFor(file);
    
    auto sampleLenght = static_cast<int>(varFormatReader->lengthInSamples);
    
    myAudioBuffer(varInt)->setSize(1, sampleLenght);
    varFormatReader->read (myAudioBuffer(varInt),0,sampleLenght, 0, true, false);
    
    String nameString = "Sample" + std::to_string(varInt);
    SamplerSound* samplerSound = mySamplerSound(varInt);
    samplerSound = new SamplerSound(nameString, *varFormatReader, myRange(varInt), myNote(varInt), 0.1, 0.1, 10.0);
    mySynthSelected(varInt)->addSound(samplerSound);
}

void Twisted_pluginAudioProcessor::loadSampleComboBox(const File& file, int& varInt)
{
    myAudioLoadedBy(varInt, 2);
    mySynthSelected(varInt)->clearSounds();
    
    varFormatReader = varFormatManager.createReaderFor(file);
    
    auto sampleLenght = static_cast<int>(varFormatReader->lengthInSamples);
    
    myAudioBuffer(varInt)->setSize(1, sampleLenght);
    varFormatReader->read (myAudioBuffer(varInt),0,sampleLenght, 0, true, false);
    
    String nameString = "Sample" + std::to_string(varInt);
    SamplerSound* samplerSound = mySamplerSound(varInt);
    samplerSound = new SamplerSound(nameString, *varFormatReader, myRange(varInt), myNote(varInt), 0.1, 0.1, 10.0);
    mySynthSelected(varInt)->addSound(samplerSound);
}

//Selecciona el sintetizador de acuerdo al ultimo elemento usado en la GUI
Synthesiser* Twisted_pluginAudioProcessor::mySynthSelected(int& varInt)
{
    switch(varInt)
    {
        case 1:
            currentSynth = 1;
            return &varSynthesiser1;
        case 2:
            currentSynth = 2;
            return &varSynthesiser2;
        case 3:
            currentSynth = 3;
            return &varSynthesiser3;
        case 4:
            currentSynth = 4;
            return &varSynthesiser4;
        default:
            currentSynth = 0;
            return &varSynthesiser0;
    }
}

//Selecciona el audioBuffer de acuerdo al ultimo elemento usado en la GUI
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
            return &varAudioBuffer0;
    }
}

//Identifica fuente del sample (1-drag o 2-combobox) de acuerdo al ultimo elemento usado en la GUI
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
            audio0LoadedBy = varInt2;
            break;
    }
}

//Controla que sinte tiene permitido escucharse de acuerdo a los botones de solo
void Twisted_pluginAudioProcessor::mySoloUpdate(int& varInt, bool varBool)
{
    if(allSolosDisabled)
    {
        solo1=false;
        solo2=false;
        solo3=false;
        solo4=false;
        allSolosDisabled=false;
    }
    
    switch(varInt)
    {
        case 1:
            solo1 = varBool;
            break;
        case 2:
            solo2 = varBool;
            break;
        case 3:
            solo3 = varBool;
            break;
        case 4:
            solo4 = varBool;
            break;
        default:
            solo0 = varBool;
            break;
    }
    
    if(!solo1&&!solo2&&!solo3&&!solo4)
    {
        solo1=true;
        solo2=true;
        solo3=true;
        solo4=true;
        allSolosDisabled=true;
    }
}

//Controla botón de Play
void Twisted_pluginAudioProcessor::myPlay(bool varBool)
{
    if(varBool)
        playButtonStarts=true;
    else
        playButtonStops=true;
}

//Informa si se ha cargado audio en el sinte indicado
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
            return varSynthesiser0.getNumSounds();
    }
    
}

//Obtiene información de variable AudioLoadedBy, la cual indica la fuente del audio
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
            return audio0LoadedBy;
    }
}

//Modifica información de variable AudioLoadedBy, la cual indica la fuente del audio
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
            audio0LoadedBy = varInt2;
            break;
    }
}

SamplerSound* Twisted_pluginAudioProcessor::mySamplerSound(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return samplerSound1;
        case 2:
            return samplerSound2;
        case 3:
            return samplerSound3;
        case 4:
            return samplerSound4;
        default:
            return samplerSound1;
    }
}

BigInteger Twisted_pluginAudioProcessor::myRange(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return range1;
        case 2:
            return range2;
        case 3:
            return range3;
        case 4:
            return range4;
        default:
            return range1;
    }
}

int Twisted_pluginAudioProcessor::myNote(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return C5note;
        case 2:
            return D5note;
        case 3:
            return E5note;
        case 4:
            return F5note;
        default:
            return C5note;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Twisted_pluginAudioProcessor();
}
