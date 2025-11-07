#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <memory>
#include <map>

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
    juce::String GetDuration();
    juce::String getPath();
    juce::String getFileName();
    void setSpeed(double newSpeed);
    double getSpeed() const { return playbackSpeed; }

    double getCurrentPosition() const;

    void setLoopPoints(double startTime, double endTime);
    void clearLoopPoints();
    void Delete();
    void setCrossfadeGain(float gain);
    juce::AudioSource* getAudioSource();

    // --- ADDED ---
    // Public getters for the loop state, so MainComponent's timer can check them.
    bool isAbLoopEngaged() const { return abLoopEngaged; }
    double getAbLoopStart() const { return abLoopStart; }
    double getAbLoopEnd() const { return abLoopEnd; }
    // --- END ADDED ---

    void addMarker(const juce::String& name, double time);
    void goToMarker(const juce::String& name);
    juce::StringArray getMarkersList() const;
    void clearAllMarkers();

    void setCurrentFile(const juce::File& file) { currentFile = file; }
    juce::File getCurrentFile() const { return currentFile; }
    std::map<juce::String, double> getMarkersMap() const { return markers; }
    void setMarkersMap(const std::map<juce::String, double>& m) { markers = m; }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::AudioTransportSource> transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplerSource;

    bool looping = false;
    double playbackSpeed = 1.0;

    juce::String filp;
    juce::String filn;

    // --- MODIFIED ---
    // Use -1.0 as a clear "unset" state for the loop start.
    double abLoopStart = -1.0;
    // --- END MODIFIED ---
    double abLoopEnd = -1.0;
    bool abLoopEngaged = false;

    float userVolume = 0.5f;
    float crossfadeGain = 1.0f;

    juce::File currentFile;
    std::map<juce::String, double> markers;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};