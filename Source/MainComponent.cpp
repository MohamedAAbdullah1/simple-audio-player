#include "MainComponent.h"

MainComponent::MainComponent()
{
    gui.onLoadFile = [this]() { handleLoadFile(); };
    gui.onRestart = [this]() { handleRestart(); };
    gui.onStop = [this]() { handleStop(); };
    gui.onStart = [this]() { handleStart(); };
    gui.onGostart = [this]() { handleGostart(); };
    gui.onGoend = [this]() { handleGoend(); };
    gui.onLoopToggled = [this](bool loop) { handleLoopToggled(loop); };
    gui.onVolumeChanged = [this](float vol) { handleVolumeChanged(vol); };
    gui.onSpeedChanged = [this](double speed) { audioPlayer.setSpeed(speed); };
    gui.onPositionChanged = [this](double pos) { handlePositionChanged(pos); };
    gui.onSetA = [this]() { handleSetA(); };
    gui.onSetB = [this]() { handleSetB(); };
    gui.onClearAB = [this]() { handleClearAB(); };

    addAndMakeVisible(gui);
    addAndMakeVisible(forwardButton);
    addAndMakeVisible(backwardButton);
    addAndMakeVisible(muteButton);
    forwardButton.setButtonText(">> 10s");
    backwardButton.setButtonText("<< 10s");
    muteButton.setButtonText("Mute");

    forwardButton.onClick = [this]() { audioPlayer.skipForward(10.0); };
    backwardButton.onClick = [this]() { audioPlayer.skipBackward(10.0); };
    muteButton.onClick = [this]() {
        audioPlayer.toggleMute();
        muteButton.setButtonText(audioPlayer.isMuted() ? "Unmute" : "Mute");
        };

    setSize(700, 350);
    setAudioChannels(0, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    audioPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    audioPlayer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    audioPlayer.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    int guiHeight = 280;
    gui.setBounds(0, 0, getWidth(), guiHeight);

    int buttonY = guiHeight + 10;
    forwardButton.setBounds(10, buttonY, 80, 30);
    backwardButton.setBounds(100, buttonY, 80, 30);
    muteButton.setBounds(190, buttonY, 80, 30);
}

void MainComponent::handleLoadFile()
{
    fileChooser = std::make_unique<juce::FileChooser>("Select an audio file...", juce::File{}, "*.wav;*.mp3;*.flac;*.ogg");
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                audioPlayer.loadFile(file);
                muteButton.setButtonText("Mute");
                gui.setPositionSliderRange(audioPlayer.getLenght());
                handleClearAB();
            }
        });
}

void MainComponent::handleRestart() { audioPlayer.restart(); }
void MainComponent::handleStart() { audioPlayer.start(); }
void MainComponent::handleStop() { audioPlayer.stop(); }
void MainComponent::handleGostart() { audioPlayer.setPosition(0.0); }
void MainComponent::handleGoend() { audioPlayer.setPosition(audioPlayer.getLenght() - 3.0f); }
void MainComponent::handleLoopToggled(bool isLooping) { audioPlayer.setLooping(isLooping); }
void MainComponent::handleVolumeChanged(float volume) { audioPlayer.setVolume(volume); }
void MainComponent::handlePositionChanged(double newPosition) { audioPlayer.setPosition(newPosition); }

void MainComponent::timerCallback()
{
    gui.setPositionSliderValue(audioPlayer.getCurrentPosition());
}

void MainComponent::handleSetA()
{
    loopStartTime = audioPlayer.getCurrentPosition();
    if (loopEndTime > 0 && loopStartTime > loopEndTime)
    {
        loopEndTime = -1.0;
    }
    audioPlayer.setLoopPoints(loopStartTime, loopEndTime);
    updateABButtons();
}

void MainComponent::handleSetB()
{
    loopEndTime = audioPlayer.getCurrentPosition();
    if (loopEndTime < loopStartTime)
    {
        loopStartTime = 0.0;
    }
    audioPlayer.setLoopPoints(loopStartTime, loopEndTime);
    updateABButtons();
}

void MainComponent::handleClearAB()
{
    loopStartTime = 0.0;
    loopEndTime = -1.0;
    audioPlayer.clearLoopPoints();
    updateABButtons();
}

void MainComponent::updateABButtons()
{
    gui.updateABButtonColors(loopStartTime > 0.0, loopEndTime > 0.0);
}