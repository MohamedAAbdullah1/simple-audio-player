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
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &startButton, &gotostartButton, &gotoendButton, &setAButton, &setBButton, &clearABButton, &forwardButton, &backwardButton })
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

    juce::Font trackFont(18.0f);
    trackFont.setBold(true);
    trackLabel.setFont(trackFont);
    trackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackLabel);

    updateABButtonColors(false, false);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawRect(getLocalBounds(), 1.0f);
}

void PlayerGUI::resized()
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    grid.templateRows.add(Track(Px(40)));
    grid.templateRows.add(Track(Px(40)));
    grid.templateRows.add(Track(Px(40)));
    grid.templateRows.add(Track(Px(40)));
    grid.templateRows.add(Track(Px(40)));

    grid.templateColumns.add(Track(Fr(2)));
    grid.templateColumns.add(Track(Fr(1)));
    grid.templateColumns.add(Track(Fr(1)));
    grid.templateColumns.add(Track(Fr(1)));
    grid.templateColumns.add(Track(Fr(1)));
    grid.templateColumns.add(Track(Fr(1)));
    grid.templateColumns.add(Track(Fr(1)));
    grid.templateColumns.add(Track(Fr(1)));

    grid.setGap(Px(5));

    grid.items.add(juce::GridItem(trackLabel).withColumn({ 1, 1 }));
    grid.items.add(juce::GridItem(loadButton).withColumn({ 2, 2 }));
    grid.items.add(juce::GridItem(restartButton).withColumn({ 3, 3 }));
    grid.items.add(juce::GridItem(startButton).withColumn({ 4, 4 }));
    grid.items.add(juce::GridItem(stopButton).withColumn({ 5, 5 }));
    grid.items.add(juce::GridItem(loopButton).withColumn({ 6, 6 }));
    grid.items.add(juce::GridItem(setAButton).withColumn({ 7, 7 }));
    grid.items.add(juce::GridItem(setBButton).withColumn({ 8, 8 }));

    grid.items.add(juce::GridItem(volumeSlider).withColumn({ 1, 6 }).withAlignSelf(juce::GridItem::AlignSelf::stretch));
    grid.items.add(juce::GridItem(clearABButton).withColumn({ 7, 7 }));
    grid.items.add(juce::GridItem(muteButton).withColumn({ 8, 8 }));

    grid.items.add(juce::GridItem(speedSlider).withColumn({ 1, 6 }).withAlignSelf(juce::GridItem::AlignSelf::stretch));
    grid.items.add(juce::GridItem(backwardButton).withColumn({ 7, 7 }));
    grid.items.add(juce::GridItem(forwardButton).withColumn({ 8, 8 }));

    grid.items.add(juce::GridItem(currentTimeLabel).withColumn({ 1, 1 }).withAlignSelf(juce::GridItem::AlignSelf::center));
    grid.items.add(juce::GridItem(positionSlider).withColumn({ 2, 7 }).withAlignSelf(juce::GridItem::AlignSelf::stretch));
    grid.items.add(juce::GridItem(totalTimeLabel).withColumn({ 8, 8 }).withAlignSelf(juce::GridItem::AlignSelf::center));

    grid.items.add(juce::GridItem(gotostartButton).withColumn({ 1, 2 }));
    grid.items.add(juce::GridItem(gotoendButton).withColumn({ 3, 4 }));

    juce::Rectangle<int> gridBounds = getLocalBounds().reduced(10);
    grid.performLayout(gridBounds);
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

void PlayerGUI::setTrackName(const juce::String& name)
{
    trackLabel.setText(name, juce::dontSendNotification);
}