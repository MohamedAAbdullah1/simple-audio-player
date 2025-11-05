#include "MainComponent.h"

MainComponent::MainComponent()
{
    gui1.onLoadFile = [this]() { handleLoadFile1(); };
    gui1.onRestart = [this]() { handleRestart1(); };
    gui1.onPlayStopClicked = [this]() { handlePlayStop1(); };
    gui1.onSkipBackward = [this]() { handleSkipBackward1(); };
    gui1.onSkipForward = [this]() { handleSkipForward1(); };
    gui1.onLoopToggled = [this](bool loop) { handleLoopToggled1(loop); };
    gui1.onVolumeChanged = [this](float vol) { handleVolumeChanged1(vol); };
    gui1.onSpeedChanged = [this](double speed) { audioPlayer1.setSpeed(speed); };
    gui1.onPositionChanged = [this](double pos) { handlePositionChanged1(pos); };
    gui1.onSetA = [this]() { handleSetA1(); };
    gui1.onSetB = [this]() { handleSetB1(); };
    gui1.onClearAB = [this]() { handleClearAB1(); };

    gui2.onLoadFile = [this]() { handleLoadFile2(); };
    gui2.onRestart = [this]() { handleRestart2(); };
    gui2.onPlayStopClicked = [this]() { handlePlayStop2(); };
    gui2.onSkipBackward = [this]() { handleSkipBackward2(); };
    gui2.onSkipForward = [this]() { handleSkipForward2(); };
    gui2.onLoopToggled = [this](bool loop) { handleLoopToggled2(loop); };
    gui2.onVolumeChanged = [this](float vol) { handleVolumeChanged2(vol); };
    gui2.onSpeedChanged = [this](double speed) { audioPlayer2.setSpeed(speed); };
    gui2.onPositionChanged = [this](double pos) { handlePositionChanged2(pos); };
    gui2.onSetA = [this]() { handleSetA2(); };
    gui2.onSetB = [this]() { handleSetB2(); };
    gui2.onClearAB = [this]() { handleClearAB2(); };

    gui1.setTrackName("Track 1");
    gui2.setTrackName("Track 2");

    gui1.onAddMarker = [this]() {
        double currentTime = audioPlayer1.getCurrentPosition();
        juce::String name = "Marker " + juce::String(audioPlayer1.getMarkersList().size() + 1)
            + " (" + juce::String(currentTime, 2) + "s)";
        audioPlayer1.addMarker(name, currentTime);
        gui1.updateMarkerList(audioPlayer1.getMarkersList());
        };
    gui1.onSelectMarker = [this](const juce::String& markerName) {
        audioPlayer1.goToMarker(markerName);
        };

    gui2.onAddMarker = [this]() {
        double currentTime = audioPlayer2.getCurrentPosition();
        juce::String name = "Marker " + juce::String(audioPlayer2.getMarkersList().size() + 1)
            + " (" + juce::String(currentTime, 2) + "s)";
        audioPlayer2.addMarker(name, currentTime);
        gui2.updateMarkerList(audioPlayer2.getMarkersList());
        };
    gui2.onSelectMarker = [this](const juce::String& markerName) {
        audioPlayer2.goToMarker(markerName);
        };

    addAndMakeVisible(gui1);
    addAndMakeVisible(gui2);

    gui1.setLookAndFeel(&globalLookAndFeel);
    gui2.setLookAndFeel(&globalLookAndFeel);

    addAndMakeVisible(crossfader);
    crossfader.setRange(0.0, 1.0);
    crossfader.setValue(0.5);
    crossfader.addListener(this);
    crossfader.setSliderStyle(juce::Slider::LinearHorizontal);
    crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    addAndMakeVisible(crossfaderLabel);
    crossfaderLabel.setText("Crossfader", juce::dontSendNotification);
    crossfaderLabel.setJustificationType(juce::Justification::centred);

    crossfader.setLookAndFeel(&globalLookAndFeel);

    setSize(1000, 500);
    setAudioChannels(0, 2);
    startTimerHz(30);

    loadSessionFromDisk();
}

MainComponent::~MainComponent()
{
    saveSessionToDisk();

    crossfader.setLookAndFeel(nullptr);
    gui1.setLookAndFeel(nullptr);
    gui2.setLookAndFeel(nullptr);
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
    g.fillAll(juce::Colour(0xff222222));
}

void MainComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();

    juce::Rectangle<int> crossfaderArea = bounds.removeFromBottom(60);

    gui1.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2).reduced(10));
    gui2.setBounds(bounds.reduced(10));

    crossfaderLabel.setBounds(crossfaderArea.removeFromTop(20));
    crossfader.setBounds(crossfaderArea.reduced(20, 0));
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &crossfader)
    {
        double val = crossfader.getValue();

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

    gui1.setPlayStopButtonState(audioPlayer1.isPlaying());
    gui2.setPlayStopButtonState(audioPlayer2.isPlaying());
}

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
                gui1.loadWaveform(file); 
                handleClearAB1();

                audioPlayer1.clearAllMarkers();
                gui1.updateMarkerList(audioPlayer1.getMarkersList());
            }
        });
}
void MainComponent::handleRestart1()
{
    audioPlayer1.restart();

    audioPlayer1.clearAllMarkers();
    gui1.updateMarkerList(audioPlayer1.getMarkersList());
}
void MainComponent::handlePlayStop1()
{
    if (audioPlayer1.isPlaying())
        audioPlayer1.stop();
    else
        audioPlayer1.start();
}
void MainComponent::handleSkipBackward1() { audioPlayer1.setPosition(std::max(0.0, audioPlayer1.getCurrentPosition() - 10.0)); }
void MainComponent::handleSkipForward1() { audioPlayer1.setPosition(std::min(audioPlayer1.getLenght(), audioPlayer1.getCurrentPosition() + 10.0)); }
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
                gui2.loadWaveform(file); 
                handleClearAB2();

                audioPlayer2.clearAllMarkers();
                gui2.updateMarkerList(audioPlayer2.getMarkersList());
            }
        });
}
void MainComponent::handleRestart2()
{
    audioPlayer2.restart();

    audioPlayer2.clearAllMarkers();
    gui2.updateMarkerList(audioPlayer2.getMarkersList());
}
void MainComponent::handlePlayStop2()
{
    if (audioPlayer2.isPlaying())
        audioPlayer2.stop();
    else
        audioPlayer2.start();
}
void MainComponent::handleSkipBackward2() { audioPlayer2.setPosition(std::max(0.0, audioPlayer2.getCurrentPosition() - 10.0)); }
void MainComponent::handleSkipForward2() { audioPlayer2.setPosition(std::min(audioPlayer2.getLenght(), audioPlayer2.getCurrentPosition() + 10.0)); }
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

void MainComponent::saveSessionToDisk()
{
    juce::String content;

    content += "TRACK1\n";
    auto file1 = audioPlayer1.getCurrentFile();
    content += (file1.existsAsFile() ? file1.getFullPathName() : juce::String()) + "\n";
    content += juce::String(audioPlayer1.getCurrentPosition()) + "\n";
    
    auto markers1 = audioPlayer1.getMarkersMap();
    for (auto& kv : markers1)
    {
        content += kv.first + "|" + juce::String(kv.second) + "\n";
    }

    content += "TRACK2\n";
    auto file2 = audioPlayer2.getCurrentFile();
    content += (file2.existsAsFile() ? file2.getFullPathName() : juce::String()) + "\n";
    content += juce::String(audioPlayer2.getCurrentPosition()) + "\n";
    auto markers2 = audioPlayer2.getMarkersMap();
    for (auto& kv : markers2)
    {
        content += kv.first + "|" + juce::String(kv.second) + "\n";
    }

    if (sessionFile.existsAsFile())
        sessionFile.replaceWithText(content);
    else
    {
        sessionFile.create();
        sessionFile.replaceWithText(content);
    }
}

void MainComponent::loadSessionFromDisk()
{
    if (!sessionFile.existsAsFile())
        return;

    juce::String content = sessionFile.loadFileAsString();
    juce::StringArray lines;
    lines.addLines(content);

    int idx = 0;
     
    if (idx < lines.size() && lines[idx].trim() == "TRACK1") idx++;
    juce::String path1 = (idx < lines.size() ? lines[idx++].trim() : juce::String());
    juce::String pos1 = (idx < lines.size() ? lines[idx++].trim() : juce::String());
    std::map<juce::String, double> markers1;
    while (idx < lines.size() && lines[idx] != "TRACK2")
    {
        auto line = lines[idx++];
        if (line.containsChar('|'))
        {
            auto parts = juce::StringArray::fromTokens(line, "|", "");
            if (parts.size() == 2) markers1[parts[0]] = parts[1].getDoubleValue();
        }
    }

    if (idx < lines.size() && lines[idx].trim() == "TRACK2") idx++;
    juce::String path2 = (idx < lines.size() ? lines[idx++].trim() : juce::String());
    juce::String pos2 = (idx < lines.size() ? lines[idx++].trim() : juce::String());
    std::map<juce::String, double> markers2;
    while (idx < lines.size())
    {
        auto line = lines[idx++];
        if (line.containsChar('|'))
        {
            auto parts = juce::StringArray::fromTokens(line, "|", "");
            if (parts.size() == 2) markers2[parts[0]] = parts[1].getDoubleValue();
        }
    }

    if (path1.isNotEmpty())
    {
        juce::File f1(path1);
        if (f1.existsAsFile())
        {
            audioPlayer1.loadFile(f1);
            gui1.setPositionSliderRange(audioPlayer1.getLenght());
            gui1.loadWaveform(f1); 
            if (pos1.isNotEmpty()) audioPlayer1.setPosition(pos1.getDoubleValue());
            audioPlayer1.setMarkersMap(markers1);
            gui1.updateMarkerList(audioPlayer1.getMarkersList());
        }
    }

    if (path2.isNotEmpty())
    {
        juce::File f2(path2);
        if (f2.existsAsFile())
        {
            audioPlayer2.loadFile(f2);
            gui2.setPositionSliderRange(audioPlayer2.getLenght());
            gui2.loadWaveform(f2); 
            if (pos2.isNotEmpty()) audioPlayer2.setPosition(pos2.getDoubleValue());
            audioPlayer2.setMarkersMap(markers2);
            gui2.updateMarkerList(audioPlayer2.getMarkersList());
        }
    }
}
