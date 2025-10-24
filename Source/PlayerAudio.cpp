#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    transportSource.setGain(lastVolume);
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

        if (muted) {
            transportSource.setGain(0.0f);
        }
        else {
            transportSource.setGain(lastVolume);
        }

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
}

void PlayerAudio::restart()
{
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::setPosition(float position) {
    transportSource.setPosition(position);
}

float PlayerAudio::getLenght() {
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setVolume(float volume)
{
    lastVolume = volume;
    if (!muted) {
        transportSource.setGain(volume);
    }
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
    muted = !muted;
    if (muted) {
        transportSource.setGain(0.0f);
    }
    else {
        transportSource.setGain(lastVolume);
    }
}

bool PlayerAudio::isMuted() const
{
    return muted;
}