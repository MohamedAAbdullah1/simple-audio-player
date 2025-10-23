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
    juce::TextButton muteButton;
    juce::TextButton forwardButton;
    juce::TextButton backwardButton;

    float previousVolume = 1.0f;

    void handleLoadFile();
    void handleRestart();
    void handleStart();
    void handleStop();
    void handleGostart();
    void handleGoend();
    void handleVolumeChanged(float volume);
    void handleLoopToggled(bool isLooping);

    PlayerAudio audioPlayer;
    PlayerGUI gui;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};