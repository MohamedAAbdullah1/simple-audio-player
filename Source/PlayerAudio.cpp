#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() : transportSource(new juce::AudioTransportSource())
{
    formatManager.registerBasicFormats();
    transportSource->setGain(userVolume * crossfadeGain);
    resamplerSource = std::make_unique<juce::ResamplingAudioSource>(transportSource.get(), false, 2);
}

PlayerAudio::~PlayerAudio()
{
    transportSource->setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resamplerSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (looping && readerSource != nullptr)
    {
        if (abLoopEngaged)
        {
            if (transportSource->getCurrentPosition() >= abLoopEnd)
            {
                transportSource->setPosition(abLoopStart);
            }
        }
        else
        {
            if (!transportSource->isPlaying() || transportSource->getCurrentPosition() >= transportSource->getLengthInSeconds())
            {
                transportSource->setPosition(0.0);
                transportSource->start();
            }
        }
    }
    resamplerSource->getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    resamplerSource->releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource->stop();
        transportSource->setSource(nullptr);
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource->setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        resamplerSource->setResamplingRatio(playbackSpeed);

        transportSource->start();
    }
}

void PlayerAudio::start() { transportSource->start(); }
void PlayerAudio::stop() { transportSource->stop(); }
void PlayerAudio::restart() { transportSource->setPosition(0.0); transportSource->start(); }
void PlayerAudio::setPosition(double position) { transportSource->setPosition(position); }
double PlayerAudio::getLenght() { return transportSource->getLengthInSeconds(); }
void PlayerAudio::setLooping(bool shouldLoop) { looping = shouldLoop; }
bool PlayerAudio::isLooping() const { return looping; }
bool PlayerAudio::isPlaying() const { return transportSource->isPlaying(); }

void PlayerAudio::setSpeed(double newSpeed)
{
    playbackSpeed = juce::jlimit(0.5, 2.0, newSpeed);
    if (resamplerSource)
        resamplerSource->setResamplingRatio(playbackSpeed);
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource->getCurrentPosition();
}

void PlayerAudio::setLoopPoints(double startTime, double endTime)
{
    abLoopStart = startTime;
    abLoopEnd = endTime;
    abLoopEngaged = (abLoopStart >= 0.0 && abLoopEnd > abLoopStart);
}

void PlayerAudio::clearLoopPoints()
{
    abLoopStart = 0.0;
    abLoopEnd = -1.0;
    abLoopEngaged = false;
}

void PlayerAudio::setVolume(float volume)
{
    userVolume = volume;
    transportSource->setGain(userVolume * crossfadeGain);
}

void PlayerAudio::setCrossfadeGain(float gain)
{
    crossfadeGain = gain;
    transportSource->setGain(userVolume * crossfadeGain);
}

juce::AudioSource* PlayerAudio::getAudioSource()
{
    return resamplerSource.get();
}