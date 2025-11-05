#include "MainComponent.h"

MainComponent::MainComponent()
{
    gui1.onLoadFile = [this]() { handleLoadFile1(); };
    gui1.onRestart = [this]() { handleRestart1(); };
    gui1.onStop = [this]() { handleStop1(); };
    gui1.onStart = [this]() { handleStart1(); };
    gui1.onGostart = [this]() { handleGostart1(); };
    gui1.onGoend = [this]() { handleGoend1(); };
    gui1.onLoopToggled = [this](bool loop) { handleLoopToggled1(loop); };
    gui1.onVolumeChanged = [this](float vol) { handleVolumeChanged1(vol); };
    gui1.onSpeedChanged = [this](double speed) { audioPlayer1.setSpeed(speed); };
    gui1.onPositionChanged = [this](double pos) { handlePositionChanged1(pos); };
    gui1.onSetA = [this]() { handleSetA1(); };
    gui1.onSetB = [this]() { handleSetB1(); };
    gui1.onClearAB = [this]() { handleClearAB1(); };

    gui2.onLoadFile = [this]() { handleLoadFile2(); };
    gui2.onRestart = [this]() { handleRestart2(); };
    gui2.onStop = [this]() { handleStop2(); };
    gui2.onStart = [this]() { handleStart2(); };
    gui2.onGostart = [this]() { handleGostart2(); };
    gui2.onGoend = [this]() { handleGoend2(); };
    gui2.onLoopToggled = [this](bool loop) { handleLoopToggled2(loop); };
    gui2.onVolumeChanged = [this](float vol) { handleVolumeChanged2(vol); };
    gui2.onSpeedChanged = [this](double speed) { audioPlayer2.setSpeed(speed); };
    gui2.onPositionChanged = [this](double pos) { handlePositionChanged2(pos); };
    gui2.onSetA = [this]() { handleSetA2(); };
    gui2.onSetB = [this]() { handleSetB2(); };
    gui2.onClearAB = [this]() { handleClearAB2(); };

    gui1.setTrackName("Track 1");
    gui2.setTrackName("Track 2");

    addAndMakeVisible(gui1);
    addAndMakeVisible(gui2);

    addAndMakeVisible(crossfader);
    crossfader.setRange(0.0, 1.0);
    crossfader.setValue(0.5);
    crossfader.addListener(this);

    addAndMakeVisible(crossfaderLabel);
    crossfaderLabel.setText("Crossfader", juce::dontSendNotification);
    crossfaderLabel.attachToComponent(&crossfader, true);

    setSize(800, 650);
    setAudioChannels(0, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    audioPlayer1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    audioPlayer2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(audioPlayer1.getAudioSource(), false);
    mixerSource.addInputSource(audioPlayer2.getAudioSource(), false);

    sliderValueChanged(&crossfader);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixerSource.releaseResources();
    audioPlayer1.releaseResources();
    audioPlayer2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    int crossfaderHeight = 50;
    int trackHeight = (getHeight() - crossfaderHeight) / 2;

    gui1.setBounds(0, 0, getWidth(), trackHeight);
    gui2.setBounds(0, trackHeight, getWidth(), trackHeight);

    crossfader.setBounds(110, getHeight() - 40, getWidth() - 130, 30);
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &crossfader)
    {
        double val = crossfader.getValue(); // 0.0 (Left/Track1) to 1.0 (Right/Track2)

        // Use "Constant Power" fade for smooth crossfade
        double gain1 = std::cos(val * juce::MathConstants<double>::halfPi);
        double gain2 = std::sin(val * juce::MathConstants<double>::halfPi);

        audioPlayer1.setCrossfadeGain((float)gain1);
        audioPlayer2.setCrossfadeGain((float)gain2);
    }
}

void MainComponent::timerCallback()
{
    gui1.setPositionSliderValue(audioPlayer1.getCurrentPosition());
    gui2.setPositionSliderValue(audioPlayer2.getCurrentPosition());
}

// --- Track 1 Handlers ---
void MainComponent::handleLoadFile1()
{
    fileChooser1 = std::make_unique<juce::FileChooser>("Select audio file for Track 1...", juce::File{}, "*.wav;*.mp3;*.flac;*.ogg");
    fileChooser1->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                audioPlayer1.loadFile(file);
                gui1.setPositionSliderRange(audioPlayer1.getLenght());
                handleClearAB1();
            }
        });
}
void MainComponent::handleRestart1() { audioPlayer1.restart(); }
void MainComponent::handleStart1() { audioPlayer1.start(); }
void MainComponent::handleStop1() { audioPlayer1.stop(); }
void MainComponent::handleGostart1() { audioPlayer1.setPosition(0.0); }
void MainComponent::handleGoend1() { audioPlayer1.setPosition(audioPlayer1.getLenght() - 3.0f); }
void MainComponent::handleLoopToggled1(bool isLooping) { audioPlayer1.setLooping(isLooping); }
void MainComponent::handleVolumeChanged1(float volume) { audioPlayer1.setVolume(volume); }
void MainComponent::handlePositionChanged1(double newPosition) { audioPlayer1.setPosition(newPosition); }
void MainComponent::handleSetA1()
{
    loopStartTime1 = audioPlayer1.getCurrentPosition();
    if (loopEndTime1 > 0 && loopStartTime1 > loopEndTime1) { loopEndTime1 = -1.0; }
    audioPlayer1.setLoopPoints(loopStartTime1, loopEndTime1);
    updateABButtons1();
}
void MainComponent::handleSetB1()
{
    loopEndTime1 = audioPlayer1.getCurrentPosition();
    if (loopEndTime1 < loopStartTime1) { loopStartTime1 = 0.0; }
    audioPlayer1.setLoopPoints(loopStartTime1, loopEndTime1);
    updateABButtons1();
}
void MainComponent::handleClearAB1()
{
    loopStartTime1 = 0.0; loopEndTime1 = -1.0;
    audioPlayer1.clearLoopPoints();
    updateABButtons1();
}
void MainComponent::updateABButtons1() { gui1.updateABButtonColors(loopStartTime1 > 0.0, loopEndTime1 > 0.0); }

// --- Track 2 Handlers ---
void MainComponent::handleLoadFile2()
{
    fileChooser2 = std::make_unique<juce::FileChooser>("Select audio file for Track 2...", juce::File{}, "*.wav;*.mp3;*.flac;*.ogg");
    fileChooser2->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                audioPlayer2.loadFile(file);
                gui2.setPositionSliderRange(audioPlayer2.getLenght());
                handleClearAB2();
            }
        });
}
void MainComponent::handleRestart2() { audioPlayer2.restart(); }
void MainComponent::handleStart2() { audioPlayer2.start(); }
void MainComponent::handleStop2() { audioPlayer2.stop(); }
void MainComponent::handleGostart2() { audioPlayer2.setPosition(0.0); }
void MainComponent::handleGoend2() { audioPlayer2.setPosition(audioPlayer2.getLenght() - 3.0f); }
void MainComponent::handleLoopToggled2(bool isLooping) { audioPlayer2.setLooping(isLooping); }
void MainComponent::handleVolumeChanged2(float volume) { audioPlayer2.setVolume(volume); }
void MainComponent::handlePositionChanged2(double newPosition) { audioPlayer2.setPosition(newPosition); }
void MainComponent::handleSetA2()
{
    loopStartTime2 = audioPlayer2.getCurrentPosition();
    if (loopEndTime2 > 0 && loopStartTime2 > loopEndTime2) { loopEndTime2 = -1.0; }
    audioPlayer2.setLoopPoints(loopStartTime2, loopEndTime2);
    updateABButtons2();
}
void MainComponent::handleSetB2()
{
    loopEndTime2 = audioPlayer2.getCurrentPosition();
    if (loopEndTime2 < loopStartTime2) { loopStartTime2 = 0.0; }
    audioPlayer2.setLoopPoints(loopStartTime2, loopEndTime2);
    updateABButtons2();
}
void MainComponent::handleClearAB2()
{
    loopStartTime2 = 0.0; loopEndTime2 = -1.0;
    audioPlayer2.clearLoopPoints();
    updateABButtons2();
}
void MainComponent::updateABButtons2() { gui2.updateABButtonColors(loopStartTime2 > 0.0, loopEndTime2 > 0.0); }