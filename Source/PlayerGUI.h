#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI();
    ~PlayerGUI() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    float getCurrentVolume() const { return volumeSlider.getValue(); }
    juce::TextButton& getMuteButton() { return muteButton; }

    void setPositionSliderRange(double maxRange);
    void setPositionSliderValue(double newValue);
    void updateABButtonColors(bool aSet, bool bSet);

    std::function<void()> onLoadFile;
    std::function<void()> onRestart;
    std::function<void()> onStart;
    std::function<void()> onStop;
    std::function<void()> onGostart;
    std::function<void()> onGoend;
    std::function<void(bool)> onLoopToggled;
    std::function<void(float)> onVolumeChanged;
    std::function<void(double)> onSpeedChanged;
    std::function<void(double)> onPositionChanged;
    std::function<void()> onSetA;
    std::function<void()> onSetB;
    std::function<void()> onClearAB;

private:
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton startButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton gotostartButton{ "Start" };
    juce::TextButton gotoendButton{ "End" };
    juce::ToggleButton loopButton{ "Loop" };

    juce::TextButton forwardButton{ ">> 10s" };
    juce::TextButton backwardButton{ "<< 10s" };
    juce::TextButton muteButton{ "Mute" };

    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::Slider speedSlider;
    juce::Label speedLabel;

    juce::Slider positionSlider;
    juce::Label currentTimeLabel;
    juce::Label totalTimeLabel;

    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton clearABButton{ "Clear A-B" };

    bool isMuted = false;
    float lastVolume = 0.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};