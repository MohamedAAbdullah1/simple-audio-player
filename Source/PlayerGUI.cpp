#include "PlayerGUI.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h> 
#include <memory>

CustomLookAndFeel::CustomLookAndFeel()
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

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    bool, bool isButtonDown)
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

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
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

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
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

class WaveformDisplay : public juce::Component,
    private juce::ChangeListener
{
public:
    WaveformDisplay()
        : cache(5), thumbnail(512, formatManager, cache)
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener(this);
    }

    void loadFile(const juce::File& file)
    {
        thumbnail.clear();
        if (file.existsAsFile())
        {
            thumbnail.setSource(new juce::FileInputSource(file));
        }
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff1b1b1b));

        auto area = getLocalBounds().toFloat().reduced(4.0f);
        g.setColour(juce::Colours::black);
        g.fillRoundedRectangle(area, 4.0f);

        if (thumbnail.getNumChannels() == 0)
        {
            g.setColour(juce::Colours::grey);
            g.drawFittedText("Waveform (no file)", getLocalBounds(), juce::Justification::centred, 1);
        }
        else
        {
            g.setColour(juce::Colour(0xff007aff)); 
            thumbnail.drawChannel(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 0, 1.0f);
        }
    }

private:
    void changeListenerCallback(juce::ChangeBroadcaster*) override { repaint(); }

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache cache;
    juce::AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

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

    addAndMakeVisible(addMarkerButton);
    addMarkerButton.addListener(this);
    addAndMakeVisible(markerList);
    markerList.addItem("No markers", 1);
    markerList.onChange = [this]() {
        if (onSelectMarker)
            onSelectMarker(markerList.getText());
        };

    updateABButtonColors(false, false);

    waveformDisplay = std::make_unique<WaveformDisplay>();
    addAndMakeVisible(waveformDisplay.get());

    setSize(400, 200);
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
    const int margin = 8;
    const int rowMargin = 5;
    const int sliderWidth = 45;        
    const int buttonHeight = 70;      
    const int markerAreaWidth = 60;
    const int waveformHeight = 60;
    const int topHeight = 30;

    const int sliderControlHeight = 100;

    const int markerButtonHeight = 25;
    const int markerListHeight = 100;

    juce::Rectangle<int> area = getLocalBounds().reduced(margin);

    juce::Rectangle<int> markerArea = area.removeFromRight(markerAreaWidth);

    addMarkerButton.setBounds(markerArea.getX() + rowMargin, markerArea.getY(), markerAreaWidth - (2 * rowMargin), markerButtonHeight);
    markerList.setBounds(markerArea.getX() + rowMargin, markerArea.getY() + markerButtonHeight + rowMargin, markerAreaWidth - (2 * rowMargin), markerListHeight);

    juce::Rectangle<int> topRow = area.removeFromTop(topHeight);

    loadButton.setBounds(topRow.removeFromRight(80).reduced(0, 2));
    trackLabel.setBounds(topRow.reduced(0, 2));

    juce::Rectangle<int> sliderRow = area.removeFromTop(sliderControlHeight);

    volumeSlider.setBounds(sliderRow.removeFromLeft(sliderWidth + rowMargin).reduced(rowMargin, 0));

    juce::Rectangle<int> speedSliderArea = sliderRow.removeFromRight(sliderWidth + rowMargin);
    speedSlider.setBounds(speedSliderArea.reduced(rowMargin, 0));

    juce::Rectangle<int> positionArea = sliderRow;
    positionSlider.setBounds(positionArea.getX(), positionArea.getY() + 10, positionArea.getWidth(), 20);

    currentTimeLabel.setBounds(positionArea.getX(), positionArea.getY() + 35, positionArea.getWidth() / 2, 20);
    totalTimeLabel.setBounds(positionArea.getX() + positionArea.getWidth() / 2, positionArea.getY() + 35, positionArea.getWidth() / 2, 20);


    int waveformY = positionArea.getY() + 35 + 20 + 2;

    if (waveformDisplay.get() != nullptr)
    {
        waveformDisplay->setBounds(
            positionArea.getX(),
            waveformY,
            positionArea.getWidth(),
            waveformHeight);
    }

    area.removeFromTop(waveformY - area.getY() + waveformHeight + rowMargin);

    juce::Rectangle<int> buttonGridArea = area.reduced(5);
    int numButtonsInRow = 9;

    int buttonWidth = (buttonGridArea.getWidth() - ((numButtonsInRow - 1) * rowMargin)) / numButtonsInRow;

    int x = buttonGridArea.getX();
    int y = buttonGridArea.getY();

    playStopButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;
    restartButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;

    setAButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;
    setBButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;
    clearABButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;

    backwardButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + rowMargin;
    forwardButton.setBounds(x, y, buttonWidth, buttonHeight);
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
        if (onVolumeChanged) onVolumeChanged(isNowMuted ? 0.0f : getCurrentVolume());
    }
    else if (button == &addMarkerButton && onAddMarker)
    {
        onAddMarker();
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

void PlayerGUI::updateMarkerList(const juce::StringArray& list)
{
    markerList.clear();
    if (list.size() == 0)
    {
        markerList.addItem("No markers", 1);
    }
    else
    {
        int id = 1;
        for (auto& name : list)
        {
            markerList.addItem(name, id++);
        }
    }
}

void PlayerGUI::loadWaveform(const juce::File& file)
{
    if (waveformDisplay)
        waveformDisplay->loadFile(file);
}
