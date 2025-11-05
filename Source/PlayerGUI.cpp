#include "PlayerGUI.h"

static juce::String formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(std::round(seconds));
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return juce::String::formatted("%02d:%02d", minutes, secs);
}

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &playStopButton, &setAButton, &setBButton, &clearABButton, &forwardButton, &backwardButton, &muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    loadButton.setButtonText("Load");
    restartButton.setButtonText("Restart");
    playStopButton.setButtonText("Play");
    setAButton.setButtonText("A");
    setBButton.setButtonText("B");
    clearABButton.setButtonText("Clear A-B");
    forwardButton.setButtonText("+10s");
    backwardButton.setButtonText("-10s");
    muteButton.setButtonText("Mute");

    loopButton.addListener(this);
    loopButton.setButtonText("Loop");
    addAndMakeVisible(loopButton);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    currentTimeLabel.setText("00:00", juce::dontSendNotification);
    currentTimeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(currentTimeLabel);

    totalTimeLabel.setText("00:00", juce::dontSendNotification);
    totalTimeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(totalTimeLabel);

    juce::Font trackFont(22.0f);
    trackFont.setBold(true);
    trackLabel.setFont(trackFont);
    trackLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(trackLabel);

    updateABButtonColors(false, false);
}

PlayerGUI::~PlayerGUI()
{
    setLookAndFeel(nullptr);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff333333));
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0f, 2.0f);
}

void PlayerGUI::resized()
{
    using FlexItem = juce::FlexItem;
    using FlexBox = juce::FlexBox;

    juce::Rectangle<int> bounds = getLocalBounds().reduced(15);

    FlexBox mainFlexBox;
    mainFlexBox.flexDirection = FlexBox::Direction::column;

    FlexBox topBox;
    topBox.flexDirection = FlexBox::Direction::row;
    topBox.items.add(FlexItem(trackLabel).withFlex(1.0f));
    topBox.items.add(FlexItem(loadButton).withWidth(100).withHeight(30));
    mainFlexBox.items.add(FlexItem(topBox).withHeight(30).withMargin(juce::FlexItem::Margin(0, 0, 10, 0)));

    FlexBox sliderBox;
    sliderBox.flexDirection = FlexBox::Direction::row;
    sliderBox.alignItems = FlexBox::AlignItems::stretch;

    sliderBox.items.add(FlexItem(volumeSlider).withWidth(30).withMargin(juce::FlexItem::Margin(0, 10, 0, 0)));

    FlexBox positionFlex;
    positionFlex.flexDirection = FlexBox::Direction::column;
    positionFlex.items.add(FlexItem(positionSlider).withHeight(20));
    FlexBox timeBox;
    timeBox.items.add(FlexItem(currentTimeLabel).withFlex(1.0f));
    timeBox.items.add(FlexItem(totalTimeLabel).withFlex(1.0f));
    positionFlex.items.add(FlexItem(timeBox).withHeight(20));

    sliderBox.items.add(FlexItem(positionFlex).withFlex(1.0f));

    sliderBox.items.add(FlexItem(speedSlider).withWidth(30).withMargin(juce::FlexItem::Margin(0, 0, 0, 10)));

    mainFlexBox.items.add(FlexItem(sliderBox).withFlex(1.0f).withMargin(juce::FlexItem::Margin(0, 0, 10, 0)));

    FlexBox buttonGrid;
    buttonGrid.flexWrap = FlexBox::Wrap::wrap;
    buttonGrid.justifyContent = FlexBox::JustifyContent::flexStart;
    buttonGrid.alignContent = FlexBox::AlignContent::stretch;

    buttonGrid.items.add(FlexItem(playStopButton).withFlex(1.0f).withMinHeight(30).withMargin(2));
    buttonGrid.items.add(FlexItem(restartButton).withFlex(1.0f).withMinHeight(30).withMargin(2));
    buttonGrid.items.add(FlexItem(loopButton).withFlex(1.0f).withMinHeight(30).withMargin(2));

    buttonGrid.items.add(FlexItem(setAButton).withFlex(1.0f).withMinHeight(30).withMargin(2));
    buttonGrid.items.add(FlexItem(setBButton).withFlex(1.0f).withMinHeight(30).withMargin(2));
    buttonGrid.items.add(FlexItem(clearABButton).withFlex(1.0f).withMinHeight(30).withMargin(2));
    buttonGrid.items.add(FlexItem(muteButton).withFlex(1.0f).withMinHeight(30).withMargin(2));

    buttonGrid.items.add(FlexItem(backwardButton).withFlex(1.0f).withMinHeight(30).withMargin(2));
    buttonGrid.items.add(FlexItem(forwardButton).withFlex(1.0f).withMinHeight(30).withMargin(2));

    mainFlexBox.items.add(FlexItem(buttonGrid).withMinHeight(100));

    mainFlexBox.performLayout(bounds);
}


void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton && onLoadFile) onLoadFile();
    else if (button == &restartButton && onRestart) onRestart();
    else if (button == &playStopButton && onPlayStopClicked) onPlayStopClicked();
    else if (button == &loopButton && onLoopToggled) onLoopToggled(loopButton.getToggleState());
    else if (button == &setAButton && onSetA) onSetA();
    else if (button == &setBButton && onSetB) onSetB();
    else if (button == &clearABButton && onClearAB) onClearAB();
    else if (button == &forwardButton && onSkipForward) onSkipForward();
    else if (button == &backwardButton && onSkipBackward) onSkipBackward();
    else if (button == &muteButton)
    {
        bool isNowMuted = !muteButton.getToggleState();
        muteButton.setToggleState(isNowMuted, juce::dontSendNotification);
        muteButton.setButtonText(isNowMuted ? "Unmute" : "Mute");
        if(onVolumeChanged) onVolumeChanged(isNowMuted ? 0.0f : getCurrentVolume());
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && onVolumeChanged)
    {
        onVolumeChanged(static_cast<float>(slider->getValue()));
        if (muteButton.getToggleState() && slider->getValue() > 0.0f)
        {
            muteButton.setToggleState(false, juce::dontSendNotification);
            muteButton.setButtonText("Mute");
        }
    }
    else if (slider == &speedSlider && onSpeedChanged) onSpeedChanged(slider->getValue());
    else if (slider == &positionSlider && onPositionChanged) onPositionChanged(slider->getValue());
}

void PlayerGUI::setPositionSliderRange(double maxRange)
{
    positionSlider.setRange(0.0, maxRange, 0.01);
    totalTimeLabel.setText(formatTime(maxRange), juce::dontSendNotification);
}

void PlayerGUI::setPositionSliderValue(double newValue)
{
    if (!positionSlider.isMouseButtonDown())
    {
        positionSlider.setValue(newValue, juce::dontSendNotification);
    }
    currentTimeLabel.setText(formatTime(newValue), juce::dontSendNotification);
}

void PlayerGUI::updateABButtonColors(bool aSet, bool bSet)
{
    setAButton.setColour(juce::TextButton::buttonColourId, aSet ? findColour(juce::TextButton::buttonOnColourId) : findColour(juce::TextButton::buttonColourId));
    setBButton.setColour(juce::TextButton::buttonColourId, bSet ? findColour(juce::TextButton::buttonOnColourId) : findColour(juce::TextButton::buttonColourId));
}

void PlayerGUI::setTrackName(const juce::String& name)
{
    trackLabel.setText(name, juce::dontSendNotification);
}

void PlayerGUI::setPlayStopButtonState(bool isPlaying)
{
    playStopButton.setButtonText(isPlaying ? "Stop" : "Play");
    playStopButton.setColour(juce::TextButton::buttonColourId, isPlaying ? findColour(juce::TextButton::buttonOnColourId) : findColour(juce::TextButton::buttonColourId));
    playStopButton.repaint();
}