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
    gui.setBounds(0, 0, getWidth(), getHeight() - 100);
    forwardButton.setBounds(10, getHeight() - 80, 80, 30);
    backwardButton.setBounds(100, getHeight() - 80, 80, 30);
    muteButton.setBounds(190, getHeight() - 80, 80, 30);
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
