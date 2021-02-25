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
    range1.setBit(C3note);
    range2.setBit(D3note);
    range3.setBit(E3note);
    range4.setBit(F3note);

    //Get Voice of each synthesiser, each one has one voice
    voiceSynth1 = static_cast<CustomSamplerVoice*>(varSynthesiser1.getVoice(0));
    voiceSynth2 = static_cast<CustomSamplerVoice*>(varSynthesiser2.getVoice(0));
    voiceSynth3 = static_cast<CustomSamplerVoice*>(varSynthesiser3.getVoice(0));
    voiceSynth4 = static_cast<CustomSamplerVoice*>(varSynthesiser4.getVoice(0));

    setLatencySamples(2050);
}

Twisted_pluginAudioProcessor::~Twisted_pluginAudioProcessor()
{
    varFormatReader = nullptr;
}

//Creating parameters layout
AudioProcessorValueTreeState::ParameterLayout Twisted_pluginAudioProcessor::paramLayout() {
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>("BOOST", "Boost", NormalisableRange<float>(49.0f, 160.0f, 1.0f), 49.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("REVERB_SNR", "ReverbSnr", NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("REVERB_HH", "ReverbHh", NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("REVERB_PERCS", "ReverbPercs", NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("LOW_EQ", "LowEQ", NormalisableRange<float>(49.0f, 1000.0f, 1.0f), 49.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("HIGH_EQ", "HighEQ", NormalisableRange<float>(1999.0f, 10000.0f, 1.0f), 1999.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("FILTER", "Filter", NormalisableRange<float>(550.0f, 10000.0f, 50.0f), 500.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("GAIN", "Gain", NormalisableRange<float>(-8.0f, 2.0f, 0.01f), -5.0f));

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

    stretcherSynth1 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1.0, 1.0);
    stretcherSynth1->reset();
    stretcherSynth2 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1.0, 1.0);
    stretcherSynth2->reset();
    stretcherSynth3 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1.0, 1.0);
    stretcherSynth3->reset();
    stretcherSynth4 = std::make_unique<RubberBand::RubberBandStretcher> (sampleRate, getTotalNumOutputChannels(), RubberBand::RubberBandStretcher::Option::OptionProcessRealTime, 1.0, 1.0);
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

    ////PLAY KEYS OF KEYBOARD
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    auto stretchingRatio = getStretchingRatio();

    ////NOTES
    checkNoteStatus(C3note, isNotePlayed1, *stretcherSynth1, *voiceSynth1, playheadValue1, stretchingRatio);
    checkNoteStatus(D3note, isNotePlayed2, *stretcherSynth2, *voiceSynth2, playheadValue2, stretchingRatio);
    checkNoteStatus(E3note, isNotePlayed3, *stretcherSynth3, *voiceSynth3, playheadValue3, stretchingRatio);
    checkNoteStatus(F3note, isNotePlayed4, *stretcherSynth4, *voiceSynth4, playheadValue4, stretchingRatio);

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

    //Apply solo/mute to each buffer
    if(playAllowed1) stretchBufferSynth1.applyGain(defaultGain);
    else stretchBufferSynth1.applyGain(0);
    if(playAllowed2) stretchBufferSynth2.applyGain(defaultGain);
    else stretchBufferSynth2.applyGain(0);
    if(playAllowed3) stretchBufferSynth3.applyGain(defaultGain);
    else stretchBufferSynth3.applyGain(0);
    if(playAllowed4) stretchBufferSynth4.applyGain(defaultGain);
    else stretchBufferSynth4.applyGain(0);

    //Adding Temporal Stretch Buffers with effects to Buffer
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {

        buffer.addFrom(channel, 0, stretchBufferSynth1, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, stretchBufferSynth2, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, stretchBufferSynth3, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, stretchBufferSynth4, channel, 0, buffer.getNumSamples());
    }

    outputEffects.processDSP(buffer); //Applying master effects
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
    auto state = apvts.copyState();

    //SAVE APVTS DATA
    /*std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);*/

    //SAVE COMBOBOXES AND SOLOS DATA
    auto switchNode = state.getOrCreateChildWithName ("myValues", nullptr);
    switchNode.setProperty ("solo1", sv1, nullptr);
    switchNode.setProperty ("solo2", sv2, nullptr);
    switchNode.setProperty ("solo3", sv3, nullptr);
    switchNode.setProperty ("solo4", sv4, nullptr);
    switchNode.setProperty ("combobox1", cbv1, nullptr);
    switchNode.setProperty ("combobox2", cbv2, nullptr);
    switchNode.setProperty ("combobox3", cbv3, nullptr);
    switchNode.setProperty ("combobox4", cbv4, nullptr);
    //SAVE COMBOBOXES AND SOLOS DATA
    switchNode.setProperty ("BOOST", knob1, nullptr);
    switchNode.setProperty ("REVERB_SNR", knob2, nullptr);
    switchNode.setProperty ("REVERB_HH", knob3, nullptr);
    switchNode.setProperty ("REVERB_PERCS", knob4, nullptr);
    switchNode.setProperty ("LOW_EQ", knob5, nullptr);
    switchNode.setProperty ("HIGH_EQ", knob6, nullptr);
    switchNode.setProperty ("FILTER", knob7, nullptr);
    switchNode.setProperty ("GAIN", knob8, nullptr);

    MemoryOutputStream stream(destData, false);
    state.writeToStream (stream);
}

void Twisted_pluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //LOAD APVTS DATA
    /*   std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

           if (xmlState.get() != nullptr)
               if (xmlState->hasTagName (apvts.state.getType()))
                   apvts.replaceState (juce::ValueTree::fromXml (*xmlState));*/

    auto state = apvts.copyState();

    //LOAD COMBOBOXES AND SOLOS DATA
    ValueTree tree = ValueTree::readFromData (data, size_t (sizeInBytes));
    if (tree.isValid())
    {
        state = tree;
        auto switchNode = state.getChildWithName ("myValues");
        if (switchNode.isValid())
        {
            if(switchNode.hasProperty ("solo1")) sv1 = switchNode.getProperty ("solo1");
            if(switchNode.hasProperty ("solo2")) sv2 = switchNode.getProperty ("solo2");
            if(switchNode.hasProperty ("solo3")) sv3 = switchNode.getProperty ("solo3");
            if(switchNode.hasProperty ("solo4")) sv4 = switchNode.getProperty ("solo4");
            if(switchNode.hasProperty ("combobox1")) cbv1 = switchNode.getProperty ("combobox1");
            if(switchNode.hasProperty ("combobox2")) cbv2 = switchNode.getProperty ("combobox2");
            if(switchNode.hasProperty ("combobox3")) cbv3 = switchNode.getProperty ("combobox3");
            if(switchNode.hasProperty ("combobox4")) cbv4 = switchNode.getProperty ("combobox4");
            if(switchNode.hasProperty ("BOOST")) knob1 = switchNode.getProperty ("BOOST");
            if(switchNode.hasProperty ("REVERB_SNR")) knob2 = switchNode.getProperty ("REVERB_SNR");
            if(switchNode.hasProperty ("REVERB_HH")) knob3 = switchNode.getProperty ("REVERB_HH");
            if(switchNode.hasProperty ("REVERB_PERCS")) knob4 = switchNode.getProperty ("REVERB_PERCS");
            if(switchNode.hasProperty ("LOW_EQ")) knob5 = switchNode.getProperty ("LOW_EQ");
            if(switchNode.hasProperty ("HIGH_EQ")) knob6 = switchNode.getProperty ("HIGH_EQ");
            if(switchNode.hasProperty ("FILTER")) knob7 = switchNode.getProperty ("FILTER");
            if(switchNode.hasProperty ("GAIN")) knob8 = switchNode.getProperty ("GAIN");
        }
    }


}

//LOAD FILES FUNCTIONS
//Read the file
void Twisted_pluginAudioProcessor::loadAFile(const File& file, int& varInt,  int comboBoxInt)
{
    myStop();
    if(comboBoxInt==0) { prepareCleanerCombobox(varInt); }
    mySynthSelected(varInt)->clearSounds();

    varFormatReader = varFormatManager.createReaderFor(file);
    auto sampleLenght = static_cast<int>(varFormatReader->lengthInSamples);
    myAudioBuffer(varInt)->setSize(1, sampleLenght);
    varFormatReader->read (myAudioBuffer(varInt),0,sampleLenght, 0, true, false);

    mySoundToSynth(varInt);
    buttonsResetProcess=true;
}

//Prepare cleaning of a bombobox
void Twisted_pluginAudioProcessor::prepareCleanerCombobox(int& varInt)
{
    switch(varInt)
    {
        case 1:
            cbcleaner1=true;
            break;
        case 2:
            cbcleaner2=true;
            break;
        case 3:
            cbcleaner3=true;
            break;
        case 4:
            cbcleaner4=true;
            break;
        default:
            break;
    }
}

//Trigger cleaning of a bombobox
int Twisted_pluginAudioProcessor::triggerCleanerCombobox()
{
    if(cbcleaner1)
    {
        cbcleaner1=false;
        return 1;
    }
    if(cbcleaner2)
    {
        cbcleaner2=false;
        return 2;
    }
    if(cbcleaner3)
    {
        cbcleaner3=false;
        return 3;
    }
    if(cbcleaner4)
    {
        cbcleaner4=false;
        return 4;
    }
    return 0;
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
    }
    return nullptr;
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
    }
    return nullptr;
}

//Assign the customSamplerSound to the corresponding Synthesiser
void Twisted_pluginAudioProcessor::mySoundToSynth(int& varInt)
{
    String nameString = "Sample" + std::to_string(varInt);
    switch(varInt)
    {
        case 1:
            customSamplerSound1 = new CustomSamplerSound(nameString, *varFormatReader, range1, C3note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound1);
            break;
        case 2:
            customSamplerSound2 = new CustomSamplerSound(nameString, *varFormatReader, range2, D3note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound2);
            break;
        case 3:
            customSamplerSound3 = new CustomSamplerSound(nameString, *varFormatReader, range3, E3note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound3);
            break;
        case 4:
            customSamplerSound4 = new CustomSamplerSound(nameString, *varFormatReader, range4, F3note, 0.1, 0.1, 10.0);
            mySynthSelected(varInt)->addSound(customSamplerSound4);
            break;
    }
}

//BUTTONS FUNCTIONS
//Buttons Play & Stop
void Twisted_pluginAudioProcessor::myPlay()
{
    keyboardState.noteOn(1, C3note, defaultGain);
    keyboardState.noteOn(1, D3note, defaultGain);
    keyboardState.noteOn(1, E3note, defaultGain);
    keyboardState.noteOn(1, F3note, defaultGain);
    isPlaying=true;
}

void Twisted_pluginAudioProcessor::myStop()
{
    isPlaying=false;
    keyboardState.allNotesOff(1);
    stretcherSynth1->reset();
    stretcherSynth2->reset();
    stretcherSynth3->reset();
    stretcherSynth4->reset();
}

//Update Solo functionality
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
}

//Reset play and stop buttons
void Twisted_pluginAudioProcessor::checkIfReset()
{
    if(!keyboardState.isNoteOn(1, 84) &&
       !keyboardState.isNoteOn(1, 86) &&
       !keyboardState.isNoteOn(1, 88) &&
       !keyboardState.isNoteOn(1, 89) )
    {
        isPlaying = false;
        buttonsResetProcess = true;
    }
}

//WAVEFORMS FUNCTIONS
//Check if a sample was loaded
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
    }
    return false;
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
    }
    return playheadValue1;
}

//Check the name of current sample, if is different it will paint a new soundwave
String Twisted_pluginAudioProcessor::getSampleName(int& varInt)
{
    switch(varInt)
    {
        case 1:
            return customSamplerSound1->getName();
        case 2:
            return customSamplerSound2->getName();
        case 3:
            return customSamplerSound3->getName();
        case 4:
            return customSamplerSound4->getName();
    }
    return "";
}

//KEYBOARD AND STRETCHING FUNCTIONS
//Get stretching ratio reading BPM
double Twisted_pluginAudioProcessor::getStretchingRatio()
{
    if (auto playHead = this->getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo positionInfo;
        playHead->getCurrentPosition(positionInfo);

        auto ratio = 75.0 / static_cast<double>(positionInfo.bpm);

        return ratio;
    }

    return 1.0;
}

//Check status of pressed notes
void Twisted_pluginAudioProcessor::checkNoteStatus(int note, std::atomic<bool>& isNotePlayed, RubberBand::RubberBandStretcher& stretcher, CustomSamplerVoice& voice, std::atomic<int>& playHeadValue, double stretchingRatio)
{
    if (keyboardState.isNoteOn(1, note))
    {
        if (!isNotePlayed)
        {
            isNotePlayed = true;
            stretcher.setTimeRatio(stretchingRatio);
            voice.setSourceSamplePos0();
        }

        playHeadValue = voice.getSourceSamplePos();

        return;
    }

    if (isNotePlayed)
    {
        isNotePlayed = false;
        playHeadValue = 0;
        stretcher.reset();
        checkIfReset();
    }
}

//REOPEN WINDOW FUNCTIONS
//Get Combobox info if Paint is destroyed
int Twisted_pluginAudioProcessor::getComboBoxInfo(int varInt)
{
    switch(varInt)
    {
        case 1:
            return cbv1;
        case 2:
            return cbv2;
        case 3:
            return cbv3;
        case 4:
            return cbv4;
    }
    return 0;
}

//Set Combobox info if Paint is destroyed
void Twisted_pluginAudioProcessor::setComboBoxInfo(int varInt, int varInt2)
{
    switch(varInt)
    {
        case 1:
            cbv1 = varInt2;
            break;
        case 2:
            cbv2 = varInt2;
            break;
        case 3:
            cbv3 = varInt2;
            break;
        case 4:
            cbv4 = varInt2;
            break;
    }
}

//Get Solo info if Paint is destroyed
bool Twisted_pluginAudioProcessor::getSoloInfo(int varInt)
{
    switch(varInt)
    {
        case 1:
            return sv1;
        case 2:
            return sv2;
        case 3:
            return sv3;
        case 4:
            return sv4;
    }
    return false;
}

//Set Solo info if Paint is destroyed
void Twisted_pluginAudioProcessor::setSoloInfo(int varInt, bool varBool)
{
    switch(varInt)
    {
        case 1:
            sv1 = varBool;
            break;
        case 2:
            sv2 = varBool;
            break;
        case 3:
            sv3 = varBool;
            break;
        case 4:
            sv4 = varBool;
            break;
    }
}

//Get APVTS info if Paint is destroyed
double Twisted_pluginAudioProcessor::getAPVTSInfo(int varInt)
{
    switch(varInt)
    {
        case 1:
            return knob1;
        case 2:
            return knob2;
        case 3:
            return knob3;
        case 4:
            return knob4;
        case 5:
            return knob5;
        case 6:
            return knob6;
        case 7:
            return knob7;
        case 8:
            return knob8;
    }
    return false;
}

//Set APVTS info if Paint is destroyed
void Twisted_pluginAudioProcessor::setAPVTSInfo(int varInt, double varDouble)
{
    switch(varInt)
    {
        case 1:
            knob1 = varDouble;
            break;
        case 2:
            knob2 = varDouble;
            break;
        case 3:
            knob3 = varDouble;
            break;
        case 4:
            knob4 = varDouble;
            break;
        case 5:
            knob5 = varDouble;
            break;
        case 6:
            knob6 = varDouble;
            break;
        case 7:
            knob7 = varDouble;
            break;
        case 8:
            knob8 = varDouble;
            break;
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Twisted_pluginAudioProcessor();
}
