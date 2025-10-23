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
    if (looping && readerSource != nullptr)
    {
        if (!transportSource.isPlaying() || transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
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
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
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
    if (!isMuted)
        transportSource.setGain(volume);

    lastVolume = volume;
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
}

bool PlayerAudio::isLooping() const
{
    return looping;
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}

void PlayerAudio::skipForward(double seconds)
{
    if (transportSource.getLengthInSeconds() > 0)
    {
        double newPosition = transportSource.getCurrentPosition() + seconds;
        if (newPosition > transportSource.getLengthInSeconds())
            newPosition = transportSource.getLengthInSeconds();
        transportSource.setPosition(newPosition);
    }
}

void PlayerAudio::skipBackward(double seconds)
{
    if (transportSource.getLengthInSeconds() > 0)
    {
        double newPosition = transportSource.getCurrentPosition() - seconds;
        if (newPosition < 0)
            newPosition = 0.0;
        transportSource.setPosition(newPosition);
    }
}

void PlayerAudio::toggleMute()
{
    if (isMuted)
    {
        transportSource.setGain(lastVolume);
        isMuted = false;
    }
    else
    {
        lastVolume = transportSource.getGain();
        transportSource.setGain(0.0f);
        isMuted = true;
    }
}