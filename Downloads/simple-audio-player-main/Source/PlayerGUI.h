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
    float getCurrentVolume() const {
        return volumeSlider.getValue();
    }


    // Callbacks for parent component
    std::function<void()> onLoadFile;
    std::function<void()> onRestart;
    std::function<void()> onStart;
    std::function<void()> onStop;
    std::function<void()> onGostart;
    std::function<void()> onGoend;
    std::function<void(bool)> onLoopToggled;
    std::function<void(float)> onVolumeChanged;

private:
    juce::TextButton loadButton { "Load Files" };
    juce::TextButton restartButton { "Restart" };
    juce::TextButton startButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton gotostartButton { "Start" };
    juce::TextButton gotoendButton { "End" };
    juce::ToggleButton loopButton {"Loop"};
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::TextButton forwardButton{ ">> 10s" };
    juce::TextButton backwardButton{ "<< 10s" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
