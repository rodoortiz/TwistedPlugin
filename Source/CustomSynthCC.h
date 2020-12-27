
#include <JuceHeader.h>
#include "DspProcessor.h"

class CustomSamplerSound    : public SynthesiserSound
{
public:
    CustomSamplerSound (const String& name,
                      AudioFormatReader& source,
                      const BigInteger& midiNotes,
                      int midiNoteForNormalPitch,
                      double attackTimeSecs,
                      double releaseTimeSecs,
                      double maxSampleLengthSeconds);

    ~CustomSamplerSound() override;

    //==============================================================================
    const String& getName() const noexcept                  { return name; }
    AudioBuffer<float>* getAudioData() const noexcept       { return data.get(); }

    void setEnvelopeParameters (ADSR::Parameters parametersToUse)    { params = parametersToUse; }

    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    //==============================================================================
    int getSourceSampleLen(){ return length; }
    
private:
    friend class CustomSamplerVoice;

    String name;
    std::unique_ptr<AudioBuffer<float>> data;
    double sourceSampleRate;
    BigInteger midiNotes;
    int length = 0, midiRootNote = 0;

    ADSR::Parameters params;

    JUCE_LEAK_DETECTOR (SamplerSound)
};

//==============================================================================

class CustomSamplerVoice    : public SynthesiserVoice
{
public:
    CustomSamplerVoice();

    ~CustomSamplerVoice() override;

    //==============================================================================
    bool canPlaySound (SynthesiserSound*) override;

    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioBuffer<float>&, int startSample, int numSamples) override;
    using SynthesiserVoice::renderNextBlock;
    
    //==============================================================================
    int getSourceSamplePos(){ return  (int)sourceSamplePosition; }
    void setVelocity(float varFloat) { lgain = varFloat; rgain = varFloat; }
    
    void setAuxBuffer(int size, int channels);
    AudioBuffer<float>& getAuxBuffer();

private:
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    float lgain = 0.75, rgain = 0.75;

    ADSR adsr;

    AudioBuffer<float> auxBuffer;
    
    JUCE_LEAK_DETECTOR (CustomSamplerVoice)
};

