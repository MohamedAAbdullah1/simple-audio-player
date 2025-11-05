#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <memory>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void loadFile(const juce::File& file);
    void start();
    void stop();
    void restart();
    void setPosition(double position);
    double getLenght();
    void setVolume(float volume);
    void setLooping(bool shouldLoop);
    bool isLooping() const;
    bool isPlaying() const;

    void setSpeed(double newSpeed);
    double getSpeed() const { return playbackSpeed; }

    double getCurrentPosition() const;

    void setLoopPoints(double startTime, double endTime);
    void clearLoopPoints();

    void setCrossfadeGain(float gain);
    juce::AudioSource* getAudioSource();

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::AudioTransportSource> transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplerSource;

    bool looping = false;
    double playbackSpeed = 1.0;

    double abLoopStart = 0.0;
    double abLoopEnd = -1.0;
    bool abLoopEngaged = false;

    float userVolume = 0.5f;
    float crossfadeGain = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};