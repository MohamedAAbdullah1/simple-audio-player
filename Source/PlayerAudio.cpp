#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
    : transportSource(new juce::AudioTransportSource())
{
    formatManager.registerBasicFormats();
    transportSource->setGain(userVolume * crossfadeGain);
    resamplerSource = std::make_unique<juce::ResamplingAudioSource>(transportSource.get(), false, 2);
}

PlayerAudio::~PlayerAudio()
{
    transportSource->setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resamplerSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (looping && readerSource != nullptr)
    {
        if (abLoopEngaged)
        {
            if (transportSource->getCurrentPosition() >= abLoopEnd)
            {
                transportSource->setPosition(abLoopStart);
            }
        }
        else
        {
            if (!transportSource->isPlaying() || transportSource->getCurrentPosition() >= transportSource->getLengthInSeconds())
            {
                transportSource->setPosition(0.0);
                transportSource->start();
            }
        }
    }
    resamplerSource->getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    resamplerSource->releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return;


    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource->stop();
        transportSource->setSource(nullptr);
        readerSource.reset();
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource->setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        resamplerSource->setResamplingRatio(playbackSpeed);
        filp=file.getFullPathName();
        filn=file.getFileName();

        currentFile = file;

        // transportSource->start();
    }
}

void PlayerAudio::start() { transportSource->start(); }
void PlayerAudio::stop() { transportSource->stop(); }
void PlayerAudio::restart() { transportSource->setPosition(0.0); transportSource->start(); }
void PlayerAudio::setPosition(double position) { transportSource->setPosition(position); }
double PlayerAudio::getLenght() { return transportSource->getLengthInSeconds(); }
void PlayerAudio::setLooping(bool shouldLoop) { looping = shouldLoop; }
bool PlayerAudio::isLooping() const { return looping; }
bool PlayerAudio::isPlaying() const { return transportSource->isPlaying(); }

juce::String PlayerAudio::GetDuration() {
    int length=getLenght();
    int time_h = length/3600;
    length-=time_h*3600;
    int time_m=length/60;
    length-=time_m*60;

    std::string hour = std::to_string(time_h);
    std::string mintut = std::to_string(time_m);
    std::string second = std::to_string(length);

    hour=(hour.size()==1?"0"+hour:hour);
    mintut=(mintut.size()==1?"0"+mintut:mintut);
    second=(second.size()==1?"0"+second:second);
    juce::String full_time = hour + ":" + mintut + ":" + second;
    return juce::String(full_time);
}

juce::String PlayerAudio::getPath() {
 return filp;
}

juce::String PlayerAudio::getFileName() {
    return filn;
}

void PlayerAudio::setSpeed(double newSpeed)
{
    playbackSpeed = juce::jlimit(0.5, 2.0, newSpeed);
    if (resamplerSource)
        resamplerSource->setResamplingRatio(playbackSpeed);
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource->getCurrentPosition();
}

void PlayerAudio::setLoopPoints(double startTime, double endTime)
{
    abLoopStart = startTime;
    abLoopEnd = endTime;
    abLoopEngaged = (abLoopStart >= 0.0 && abLoopEnd > abLoopStart);
}

void PlayerAudio::clearLoopPoints()
{
    abLoopStart = 0.0;
    abLoopEnd = -1.0;
    abLoopEngaged = false;
}

void PlayerAudio::Delete() {
    transportSource->stop();
    transportSource->setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::setVolume(float volume)
{
    userVolume = volume;
    transportSource->setGain(userVolume * crossfadeGain);
}

void PlayerAudio::setCrossfadeGain(float gain)
{
    crossfadeGain = gain;
    transportSource->setGain(userVolume * crossfadeGain);
}

juce::AudioSource* PlayerAudio::getAudioSource()
{
    return resamplerSource.get();
}

void PlayerAudio::addMarker(const juce::String& name, double time)
{
    markers[name] = time;
}

void PlayerAudio::goToMarker(const juce::String& name)
{
    auto it = markers.find(name);
    if (it != markers.end())
    {
        setPosition(it->second);
    }
}

juce::StringArray PlayerAudio::getMarkersList() const
{
    juce::StringArray arr;
    for (auto& kv : markers)
        arr.add(kv.first);
    return arr;
}

void PlayerAudio::clearAllMarkers()
{
    markers.clear();
}

