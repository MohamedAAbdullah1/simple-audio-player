#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Slider::Listener,
                      public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

private:
    juce::MixerAudioSource mixerSource;
    PlayerAudio audioPlayer1;
    PlayerAudio audioPlayer2;

    PlayerGUI gui1;
    PlayerGUI gui2;
    juce::Slider crossfader;
    juce::Label crossfaderLabel;

    CustomLookAndFeel globalLookAndFeel;

    std::unique_ptr<juce::FileChooser> fileChooser1;
    std::unique_ptr<juce::FileChooser> fileChooser2;

    double loopStartTime1 = 0.0;
    double loopEndTime1 = -1.0;

    double loopStartTime2 = 0.0;
    double loopEndTime2 = -1.0;

    void handleLoadFile1();
    void handleRestart1();
    void handlePlayStop1();
    void handleSkipBackward1();
    void handleSkipForward1();
    void handleLoopToggled1(bool isLooping);
    void handleVolumeChanged1(float volume);
    void handleSpeedChanged1(double speed);
    void handlePositionChanged1(double newPosition);
    void handleSetA1();
    void handleSetB1();
    void handleClearAB1();
    void updateABButtons1();

    void handleLoadFile2();
    void handleRestart2();
    void handlePlayStop2();
    void handleSkipBackward2();
    void handleSkipForward2();
    void handleLoopToggled2(bool isLooping);
    void handleVolumeChanged2(float volume);
    void handleSpeedChanged2(double speed);
    void handlePositionChanged2(double newPosition);
    void handleSetA2();
    void handleSetB2();
    void handleClearAB2();
    void updateABButtons2();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};