#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <vector>
#include <map>
#include <functional>

class MidiProcessor : public juce::MidiInputCallback
{
public:
    MidiProcessor();
    ~MidiProcessor() override;

    // Setup
    void prepareToPlay(double sampleRate);
    void releaseResources();

    // MIDI input callback
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

    // MIDI device management
    void setMidiInputDevice(const juce::String& deviceName);
    void setMidiOutputDevice(const juce::String& deviceName);
    juce::StringArray getMidiInputDevices() const;
    juce::StringArray getMidiOutputDevices() const;

    // MIDI control
    void sendMidiMessage(const juce::MidiMessage& message);
    void sendNoteOn(int channel, int note, int velocity);
    void sendNoteOff(int channel, int note);
    void sendControlChange(int channel, int controller, int value);
    void sendProgramChange(int channel, int program);

    // MIDI learn functionality
    void enableMidiLearn(bool enabled);
    bool isMidiLearnEnabled() const { return midiLearnEnabled; }
    void setMidiLearnCallback(std::function<void(int, int, int)> callback);

    // MIDI mapping
    void mapMidiControl(int midiController, int parameterId);
    void unmapMidiControl(int midiController);
    int getMappedParameter(int midiController) const;

    // MIDI clock and sync
    void setMidiClockEnabled(bool enabled);
    bool isMidiClockEnabled() const { return midiClockEnabled; }
    double getBPM() const { return currentBPM; }
    int getMidiClockCount() const { return midiClockCount; }

    // MIDI recording
    void startMidiRecording();
    void stopMidiRecording();
    bool isRecording() const { return isMidiRecording; }
    std::vector<juce::MidiMessage> getRecordedMidi() const;

    // MIDI playback
    void loadMidiFile(const juce::File& file);
    void playMidiFile();
    void stopMidiPlayback();
    bool isPlaying() const { return isMidiPlaying; }

private:
    // MIDI devices
    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;
    juce::String currentInputDevice;
    juce::String currentOutputDevice;

    // MIDI learn
    bool midiLearnEnabled = false;
    std::function<void(int, int, int)> midiLearnCallback;

    // MIDI mapping
    std::map<int, int> midiControlMap; // controller -> parameter

    // MIDI clock
    bool midiClockEnabled = false;
    double currentBPM = 120.0;
    int midiClockCount = 0;
    double sampleRate = 44100.0;

    // MIDI recording
    bool isMidiRecording = false;
    std::vector<juce::MidiMessage> recordedMidi;
    double recordingStartTime = 0.0;

    // MIDI playback
    bool isMidiPlaying = false;
    std::vector<juce::MidiMessage> midiFileData;
    size_t currentMidiIndex = 0;
    double playbackStartTime = 0.0;

    // Helper methods
    void processMidiMessage(const juce::MidiMessage& message);
    void updateMidiClock(const juce::MidiMessage& message);
    void handleMidiLearn(const juce::MidiMessage& message);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiProcessor)
}; 