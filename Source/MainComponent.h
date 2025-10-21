#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::TextButton forwardButton;
    juce::TextButton backwardButton;
    juce::TextButton muteButton;

    bool isMuted = false;
    float previousVolume = 1.0f;

    void handleLoadFile();
    void handleRestart();
    void handleStop();
    void handleVolumeChanged(float volume);

    PlayerAudio audioPlayer;
    PlayerGUI gui;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};