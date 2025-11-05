#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class WaveformDisplay;

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
        bool, bool isButtonDown) override;
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;
};

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

    float getCurrentVolume() const { return (float)volumeSlider.getValue(); }

    void setPositionSliderRange(double maxRange);
    void setPositionSliderValue(double newValue);
    void updateABButtonColors(bool aSet, bool bSet);
    void setTrackName(const juce::String& name);
    void setPlayStopButtonState(bool isPlaying);

    std::function<void()> onAddMarker;
    std::function<void(const juce::String&)> onSelectMarker;

    std::function<void()> onLoadFile;
    std::function<void()> onRestart;
    std::function<void()> onPlayStopClicked;
    std::function<void()> onSkipBackward;
    std::function<void()> onSkipForward;
    std::function<void(bool)> onLoopToggled;
    std::function<void(float)> onVolumeChanged;
    std::function<void(double)> onSpeedChanged;
    std::function<void(double)> onPositionChanged;
    std::function<void()> onSetA;
    std::function<void()> onSetB;
    std::function<void()> onClearAB;

    void updateMarkerList(const juce::StringArray& list);

    void loadWaveform(const juce::File& file);

private:
    juce::TextButton loadButton;
    juce::TextButton restartButton;
    juce::TextButton playStopButton;
    juce::ToggleButton loopButton;
    juce::TextButton setAButton;
    juce::TextButton setBButton;
    juce::TextButton clearABButton;

    juce::TextButton forwardButton;
    juce::TextButton backwardButton;
    juce::TextButton muteButton;

    juce::Slider volumeSlider;
    juce::Slider speedSlider;

    juce::Slider positionSlider;
    juce::Label currentTimeLabel;
    juce::Label totalTimeLabel;
    juce::Label trackLabel;

    juce::TextButton addMarkerButton{ "Add Marker" };
    juce::ComboBox markerList;

    std::unique_ptr<WaveformDisplay> waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
