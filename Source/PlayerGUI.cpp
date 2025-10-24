#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton,&startButton,&gotostartButton,&gotoendButton })
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
}

PlayerGUI::~PlayerGUI()
{
}

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
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton && onLoadFile)
        onLoadFile();
    else if (button == &restartButton && onRestart)
        onRestart();
    else if (button == &startButton && onStart)
        onStart();
    else if (button == &stopButton && onStop)
        onStop();
    else if (button == &gotostartButton && onGostart)
        onGostart();
    else if (button == &gotoendButton && onGoend)
        onGoend();
    else if (button == &loopButton && onLoopToggled)
        onLoopToggled(loopButton.getToggleState());
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && onVolumeChanged)
        onVolumeChanged(static_cast<float>(slider->getValue()));
}