
#include <JuceHeader.h>
#include "WaveFormCC.h"

//==============================================================================
WaveFormCC::WaveFormCC(Twisted_pluginAudioProcessor& p) :  audioProcessor(p)
{

}

WaveFormCC::~WaveFormCC()
{
}

std::vector<std::unique_ptr<WaveFormCC>> WaveFormCC::createObjects(Twisted_pluginAudioProcessor& p)
{
    auto v = std::vector<std::unique_ptr<WaveFormCC>>();
    
    for (auto i = 0; i < 4; i++)
    {
        v.emplace_back(std::make_unique<WaveFormCC>(p));
        v[i]->setDrumSamplerType(i+1);
    };
    
    return v;
}

void WaveFormCC::paint (juce::Graphics& g)
{
    g.fillAll(Colours::darkgrey.darker());
    
    //DRAG AND DROP
    AudioBuffer<float> waveform = myWaveForm();

    if(audioProcessor.myGetNumSamplerSounds(drumSamplerType)>0)
    {
        Path p;
        mAudioPoints.clear();
        
        auto ratio = waveform.getNumSamples()/getWidth();
        auto buffer = waveform.getReadPointer(0);
        
        //scale audio file to window on x axis
        for(int sample = 0; sample < waveform.getNumSamples(); sample+=ratio)
        {
            mAudioPoints.push_back (buffer[sample]);
        }
        
        g.setColour(Colours::cadetblue.brighter());
        p.startNewSubPath(0, getHeight()/2);
        
        //scale audio file to window on y axis
        for(int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto point = jmap<float> (mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0);
            p.lineTo (sample, point);
        }
        
        g.strokePath(p, PathStrokeType(2));
        
        //PLAYHEAD
        auto playHeadPosition = jmap<int> (audioProcessor.myGetPlayheadValue(drumSamplerType), 0, myWaveForm().getNumSamples(), 0, getWidth());
        g.setColour(Colours::white);
        g.drawLine(playHeadPosition, 0, playHeadPosition, getHeight(), 2.0f);
        
        g.setColour(Colours::black.withAlpha(0.2f));
        g.fillRect(0, 0, playHeadPosition, getHeight());
    }
    else
    {
        g.setColour(Colours::white);
        g.setFont(20.0f);
        g.drawFittedText("Load Sample", getLocalBounds(), Justification::centred, 1);
    }
    
}

void WaveFormCC::resized()
{
    
}

//OWN CODE
bool WaveFormCC::isInterestedInFileDrag(const StringArray& files)
{
    for (auto file : files)
    {
        if(file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
            return true;
    }
    return false;
}

void WaveFormCC::filesDropped (const StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(files))
        {  
            auto theFile = File(file);
            audioProcessor.loadAFile(theFile, drumSamplerType, 1);
        }
    }
    
    repaint();
}

void WaveFormCC::setDrumSamplerType(int varInt)
{
    drumSamplerType = varInt;
}

//Selecciona el audioBuffer de acuerdo al ultimo elemento usado en la GUI
AudioBuffer<float> WaveFormCC::myWaveForm()
{
    switch(drumSamplerType)
    {
        case 1:
            return audioProcessor.varAudioBuffer1;
        case 2:
            return audioProcessor.varAudioBuffer2;
        case 3:
            return audioProcessor.varAudioBuffer3;
        case 4:
            return audioProcessor.varAudioBuffer4;
        default:
            return audioProcessor.varAudioBuffer1;
    }
}


