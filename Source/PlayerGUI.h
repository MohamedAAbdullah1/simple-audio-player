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

    // Callbacks for parent component
    std::function<void()> onLoadFile;
    std::function<void()> onRestart;
    std::function<void()> onStop;
    std::function<void(float)> onVolumeChanged;

private:
    juce::TextButton loadButton { "Load Files" };
    juce::TextButton restartButton { "Restart" };
    juce::TextButton stopButton { "Stop" };
    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};