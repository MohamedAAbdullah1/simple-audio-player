#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xff007aff));
        setColour(juce::Slider::trackColourId, juce::Colour(0xff444444));
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xff444444));

        setColour(juce::TextButton::buttonColourId, juce::Colour(0xff444444));
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff007aff));
        setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);

        setColour(juce::ToggleButton::textColourId, juce::Colours::lightgrey);
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
        setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff007aff));
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool, bool isButtonDown) override
    {
        (void)backgroundColour;
        auto bounds = button.getLocalBounds().toFloat();
        auto cornerSize = 6.0f;

        juce::Colour baseColour = button.findColour(juce::TextButton::buttonColourId);

        if (isButtonDown)
            baseColour = baseColour.darker(0.3f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        (void)shouldDrawButtonAsHighlighted;

        auto bounds = button.getLocalBounds().toFloat();
        auto cornerSize = 6.0f;

        juce::Colour baseColour = findColour(juce::TextButton::buttonColourId);
        if (button.getToggleState())
            baseColour = findColour(juce::TextButton::buttonOnColourId);
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.3f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds.reduced(2.0f), cornerSize);

        g.setColour(button.findColour(juce::ToggleButton::textColourId));
        g.setFont(14.0f);
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred, 1.0f);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        (void)minSliderPos;
        (void)maxSliderPos;

        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
        auto cornerSize = 4.0f;

        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(slider.findColour(juce::Slider::trackColourId));
        if (style == juce::Slider::LinearHorizontal)
        {
            g.fillRoundedRectangle(bounds.withWidth(sliderPos - (float)x), cornerSize);
        }
        else if (style == juce::Slider::LinearVertical)
        {
            g.fillRoundedRectangle(bounds.withTop(sliderPos), cornerSize);
        }

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        if (style == juce::Slider::LinearHorizontal)
        {
            g.fillRect(juce::Rectangle<float>(sliderPos - 3.0f, (float)y, 6.0f, (float)height));
        }
        else if (style == juce::Slider::LinearVertical)
        {
            g.fillRect(juce::Rectangle<float>((float)x, sliderPos - 3.0f, (float)width, 6.0f));
        }
    }
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

    float getCurrentVolume() const { return volumeSlider.getValue(); }

    void setPositionSliderRange(double maxRange);
    void setPositionSliderValue(double newValue);
    void updateABButtonColors(bool aSet, bool bSet);
    void setTrackName(const juce::String& name);
    void setPlayStopButtonState(bool isPlaying);

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};