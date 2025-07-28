#include "MidiProcessor.h"

MidiProcessor::MidiProcessor()
{
}

MidiProcessor::~MidiProcessor()
{
    releaseResources();
}

void MidiProcessor::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void MidiProcessor::releaseResources()
{
    if (midiInput)
    {
        midiInput->stop();
        midiInput.reset();
    }
    
    if (midiOutput)
    {
        midiOutput.reset();
    }
}

void MidiProcessor::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    processMidiMessage(message);
}

void MidiProcessor::setMidiInputDevice(const juce::String& deviceName)
{
    if (midiInput)
    {
        midiInput->stop();
        midiInput.reset();
    }

    if (deviceName.isNotEmpty())
    {
        midiInput = juce::MidiInput::openDevice(deviceName, this);
        if (midiInput)
        {
            midiInput->start();
            currentInputDevice = deviceName;
        }
    }
}

void MidiProcessor::setMidiOutputDevice(const juce::String& deviceName)
{
    if (midiOutput)
    {
        midiOutput.reset();
    }

    if (deviceName.isNotEmpty())
    {
        midiOutput = juce::MidiOutput::openDevice(deviceName);
        currentOutputDevice = deviceName;
    }
}

juce::StringArray MidiProcessor::getMidiInputDevices() const
{
    return juce::MidiInput::getDevices();
}

juce::StringArray MidiProcessor::getMidiOutputDevices() const
{
    return juce::MidiOutput::getDevices();
}

void MidiProcessor::sendMidiMessage(const juce::MidiMessage& message)
{
    if (midiOutput)
    {
        midiOutput->sendMessageNow(message);
    }
}

void MidiProcessor::sendNoteOn(int channel, int note, int velocity)
{
    juce::MidiMessage message = juce::MidiMessage::noteOn(channel, note, (juce::uint8)velocity);
    sendMidiMessage(message);
}

void MidiProcessor::sendNoteOff(int channel, int note)
{
    juce::MidiMessage message = juce::MidiMessage::noteOff(channel, note);
    sendMidiMessage(message);
}

void MidiProcessor::sendControlChange(int channel, int controller, int value)
{
    juce::MidiMessage message = juce::MidiMessage::controllerEvent(channel, controller, value);
    sendMidiMessage(message);
}

void MidiProcessor::sendProgramChange(int channel, int program)
{
    juce::MidiMessage message = juce::MidiMessage::programChange(channel, program);
    sendMidiMessage(message);
}

void MidiProcessor::enableMidiLearn(bool enabled)
{
    midiLearnEnabled = enabled;
}

void MidiProcessor::setMidiLearnCallback(std::function<void(int, int, int)> callback)
{
    midiLearnCallback = callback;
}

void MidiProcessor::mapMidiControl(int midiController, int parameterId)
{
    midiControlMap[midiController] = parameterId;
}

void MidiProcessor::unmapMidiControl(int midiController)
{
    midiControlMap.erase(midiController);
}

int MidiProcessor::getMappedParameter(int midiController) const
{
    auto it = midiControlMap.find(midiController);
    return it != midiControlMap.end() ? it->second : -1;
}

void MidiProcessor::setMidiClockEnabled(bool enabled)
{
    midiClockEnabled = enabled;
    if (!enabled)
    {
        midiClockCount = 0;
        currentBPM = 120.0;
    }
}

void MidiProcessor::startMidiRecording()
{
    isMidiRecording = true;
    recordedMidi.clear();
    recordingStartTime = juce::Time::getMillisecondCounterHiRes();
}

void MidiProcessor::stopMidiRecording()
{
    isMidiRecording = false;
}

std::vector<juce::MidiMessage> MidiProcessor::getRecordedMidi() const
{
    return recordedMidi;
}

void MidiProcessor::loadMidiFile(const juce::File& file)
{
    juce::MidiFile midiFile;
    if (midiFile.readFrom(file))
    {
        midiFileData.clear();
        
        for (int track = 0; track < midiFile.getNumTracks(); ++track)
        {
            const juce::MidiMessageSequence* trackSequence = midiFile.getTrack(track);
            for (int event = 0; event < trackSequence->getNumEvents(); ++event)
            {
                const juce::MidiMessageSequence::MidiEventHolder* eventHolder = trackSequence->getEventPointer(event);
                midiFileData.push_back(eventHolder->message);
            }
        }
        
        currentMidiIndex = 0;
    }
}

void MidiProcessor::playMidiFile()
{
    if (!midiFileData.empty())
    {
        isMidiPlaying = true;
        playbackStartTime = juce::Time::getMillisecondCounterHiRes();
        currentMidiIndex = 0;
    }
}

void MidiProcessor::stopMidiPlayback()
{
    isMidiPlaying = false;
    currentMidiIndex = 0;
}

void MidiProcessor::processMidiMessage(const juce::MidiMessage& message)
{
    // Record MIDI if recording
    if (isMidiRecording)
    {
        double timestamp = juce::Time::getMillisecondCounterHiRes() - recordingStartTime;
        juce::MidiMessage recordedMessage = message;
        recordedMessage.setTimeStamp(timestamp);
        recordedMidi.push_back(recordedMessage);
    }

    // Handle MIDI learn
    if (midiLearnEnabled)
    {
        handleMidiLearn(message);
    }

    // Handle MIDI clock
    if (midiClockEnabled)
    {
        updateMidiClock(message);
    }

    // Handle mapped controls
    if (message.isController())
    {
        int controller = message.getControllerNumber();
        int value = message.getControllerValue();
        
        auto it = midiControlMap.find(controller);
        if (it != midiControlMap.end())
        {
            // Convert MIDI value (0-127) to normalized value (0.0-1.0)
            float normalizedValue = value / 127.0f;
            
            // Here you would typically call a callback to update the parameter
            // For now, we'll just store the mapping
        }
    }
}

void MidiProcessor::updateMidiClock(const juce::MidiMessage& message)
{
    if (message.isMidiClock())
    {
        midiClockCount++;
        
        // Calculate BPM from MIDI clock (24 MIDI clocks per quarter note)
        if (midiClockCount % 24 == 0)
        {
            // This is a simplified BPM calculation
            // In a real implementation, you'd track timing more precisely
            currentBPM = 120.0; // Placeholder
        }
    }
    else if (message.isMidiStart())
    {
        midiClockCount = 0;
    }
    else if (message.isMidiStop())
    {
        midiClockCount = 0;
    }
}

void MidiProcessor::handleMidiLearn(const juce::MidiMessage& message)
{
    if (message.isController() && midiLearnCallback)
    {
        int controller = message.getControllerNumber();
        int value = message.getControllerValue();
        
        // Call the learn callback with controller number and value
        midiLearnCallback(controller, value, 0); // 0 = learn mode
    }
} 