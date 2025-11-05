#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Timer,
                      public juce::Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    PlayerAudio audioPlayer1;
    PlayerGUI gui1;
    std::unique_ptr<juce::FileChooser> fileChooser1;
    double loopStartTime1 = 0.0;
    double loopEndTime1 = -1.0;

    PlayerAudio audioPlayer2;
    PlayerGUI gui2;
    std::unique_ptr<juce::FileChooser> fileChooser2;
    double loopStartTime2 = 0.0;
    double loopEndTime2 = -1.0;

    juce::MixerAudioSource mixerSource;
    juce::Slider crossfader;
    juce::Label crossfaderLabel;

    void handleLoadFile1();
    void handleRestart1();
    void handleStart1();
    void handleStop1();
    void handleGostart1();
    void handleGoend1();
    void handleVolumeChanged1(float volume);
    void handleLoopToggled1(bool isLooping);
    void handlePositionChanged1(double newPosition);
    void handleSetA1();
    void handleSetB1();
    void handleClearAB1();
    void updateABButtons1();

    void handleLoadFile2();
    void handleRestart2();
    void handleStart2();
    void handleStop2();
    void handleGostart2();
    void handleGoend2();
    void handleVolumeChanged2(float volume);
    void handleLoopToggled2(bool isLooping);
    void handlePositionChanged2(double newPosition);
    void handleSetA2();
    void handleSetB2();
    void handleClearAB2();
    void updateABButtons2();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};