#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    transportSource.setGain(lastVolume);
    resamplerSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
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
        if (!transportSource.isPlaying() || transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
        {
            transportSource.setPosition(0.0);
            transportSource.start();
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
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        resamplerSource->setResamplingRatio(playbackSpeed);

        transportSource.start();
    }
}

void PlayerAudio::start() { transportSource.start(); }
void PlayerAudio::stop() { transportSource.stop(); }
void PlayerAudio::restart() { transportSource.setPosition(0.0); transportSource.start(); }
void PlayerAudio::setPosition(float position) { transportSource.setPosition(position); }
float PlayerAudio::getLenght() { return transportSource.getLengthInSeconds(); }
void PlayerAudio::setVolume(float volume) { lastVolume = volume; if (!muted) transportSource.setGain(volume); }
void PlayerAudio::setLooping(bool shouldLoop) { looping = shouldLoop; }
bool PlayerAudio::isLooping() const { return looping; }
bool PlayerAudio::isPlaying() const { return transportSource.isPlaying(); }
void PlayerAudio::skipForward(double seconds) { transportSource.setPosition(std::min(transportSource.getCurrentPosition() + seconds, transportSource.getLengthInSeconds())); }
void PlayerAudio::skipBackward(double seconds) { transportSource.setPosition(std::max(transportSource.getCurrentPosition() - seconds, 0.0)); }
void PlayerAudio::toggleMute() { muted = !muted; transportSource.setGain(muted ? 0.0f : lastVolume); }
bool PlayerAudio::isMuted() const { return muted; }

void PlayerAudio::setSpeed(double newSpeed)
{
    playbackSpeed = juce::jlimit(0.5, 2.0, newSpeed);
    if (resamplerSource)
        resamplerSource->setResamplingRatio(playbackSpeed);
}
