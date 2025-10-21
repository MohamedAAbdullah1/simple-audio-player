#include "MainComponent.h"

MainComponent::MainComponent()
{
    // Setup GUI callbacks
    gui.onLoadFile = [this]() { handleLoadFile(); };
    gui.onRestart = [this]() { handleRestart(); };
    gui.onStop = [this]() { handleStop(); };
    gui.onVolumeChanged = [this](float vol) { handleVolumeChanged(vol); };

    addAndMakeVisible(gui);
    setSize(500, 250);
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
    gui.setBounds(getLocalBounds());
}

void MainComponent::handleLoadFile()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...",
        juce::File{},
        "*.wav;*.mp3;*.flac;*.ogg");

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                audioPlayer.loadFile(file);
            }
        });
}

void MainComponent::handleRestart()
{
    audioPlayer.restart();
}

void MainComponent::handleStop()
{
    audioPlayer.stop();
}

void MainComponent::handleVolumeChanged(float volume)
{
    audioPlayer.setVolume(volume);
}