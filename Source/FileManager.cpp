
#include <JuceHeader.h>

struct FileManager
{
    enum class SamplesFolder
    {
        Kick,
        Snare,
        HiHats,
        Percussion
    };

    static juce::File createDirectory(const juce::File& file)
    {
        if (!file.exists())
            file.createDirectory();
        return file;
    };

    static juce::File getApplicationDataDirectory()
    {
        auto file = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Application Support")
        .getChildFile("Twisted Plugin");

        return createDirectory(file);
    };

    static juce::File getSamplesDirectory()
    {
        auto file = getApplicationDataDirectory().getChildFile("Samples");

        return createDirectory(file);
    };

    static juce::File getSampleDirectory(SamplesFolder folder)
    {
        auto directory = getSamplesDirectory();

        if (folder == SamplesFolder::Kick)
            return createDirectory(directory.getChildFile("Kick"));

        if (folder == SamplesFolder::Snare)
            return createDirectory(directory.getChildFile("Snare"));

        if (folder == SamplesFolder::HiHats)
            return createDirectory(directory.getChildFile("HiHats"));

        if (folder == SamplesFolder::Percussion)
            return createDirectory(directory.getChildFile("Percussion"));
    };
    
  
};
