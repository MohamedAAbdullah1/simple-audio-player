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
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &startButton, &gotostartButton, &gotoendButton, &setAButton, &setBButton, &clearABButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    addAndMakeVisible(muteButton);
    muteButton.setButtonText("Mute");
    muteButton.addListener(this);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    addAndMakeVisible(forwardButton);
    addAndMakeVisible(backwardButton);
    forwardButton.addListener(this);
    backwardButton.addListener(this);

    volumeLabel.setText("Volume:", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, true);
    addAndMakeVisible(volumeLabel);


    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed:", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, true);
    addAndMakeVisible(speedLabel);

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

    updateABButtonColors(false, false);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    int margin = 20;

    loadButton.setBounds(margin, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    startButton.setBounds(240, y, 80, 40);
    stopButton.setBounds(340, y, 80, 40);
    gotostartButton.setBounds(440, y, 80, 40);
    gotoendButton.setBounds(540, y, 80, 40);
    loopButton.setBounds(640, y, 80, 40);

    volumeSlider.setBounds(70, 100, getWidth() - 90, 30);
    speedSlider.setBounds(70, 150, getWidth() - 90, 30);

    int yPos = 200;
    currentTimeLabel.setBounds(10, yPos, 60, 30);
    positionSlider.setBounds(70, yPos, getWidth() - 140, 30);
    totalTimeLabel.setBounds(getWidth() - 70, yPos, 60, 30);

    int yPosButtons = yPos + 40;
    setAButton.setBounds(margin, yPosButtons, 80, 30);
    setBButton.setBounds(margin + 90, yPosButtons, 80, 30);
    clearABButton.setBounds(margin + 180, yPosButtons, 100, 30);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton && onLoadFile) onLoadFile();
    else if (button == &restartButton && onRestart) onRestart();
    else if (button == &startButton && onStart) onStart();
    else if (button == &stopButton && onStop) onStop();
    else if (button == &gotostartButton && onGostart) onGostart();
    else if (button == &gotoendButton && onGoend) onGoend();
    else if (button == &loopButton && onLoopToggled) onLoopToggled(loopButton.getToggleState());
    else if (button == &setAButton && onSetA) onSetA();
    else if (button == &setBButton && onSetB) onSetB();
    else if (button == &clearABButton && onClearAB) onClearAB();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && onVolumeChanged) onVolumeChanged(static_cast<float>(slider->getValue()));
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
    positionSlider.setValue(newValue, juce::dontSendNotification);
    currentTimeLabel.setText(formatTime(newValue), juce::dontSendNotification);
}

void PlayerGUI::updateABButtonColors(bool aSet, bool bSet)
{
    auto& laf = getLookAndFeel();
    setAButton.setColour(juce::TextButton::buttonColourId, aSet ? laf.findColour(juce::TextButton::buttonOnColourId) : laf.findColour(juce::TextButton::buttonColourId));
    setBButton.setColour(juce::TextButton::buttonColourId, bSet ? laf.findColour(juce::TextButton::buttonOnColourId) : laf.findColour(juce::TextButton::buttonColourId));
}