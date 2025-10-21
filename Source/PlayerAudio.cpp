#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        // Disconnect old source first
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // Create new reader source
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        // Attach safely
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        transportSource.start();
    }
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::restart()
{
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::setVolume(float volume)
{
    transportSource.setGain(volume);
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}