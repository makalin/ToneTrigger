#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

class MidiProcessor;

class MidiSettingsPanel : public juce::Component
{
public:
    MidiSettingsPanel();
    ~MidiSettingsPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // MIDI processor connection
    void setMidiProcessor(MidiProcessor* processor);
    void updateDeviceLists();

    // Callbacks
    void setMidiLearnCallback(std::function<void(bool)> callback);
    void setMidiClockCallback(std::function<void(bool)> callback);

private:
    // MIDI processor
    MidiProcessor* midiProcessor = nullptr;

    // UI components
    juce::ComboBox midiInputCombo;
    juce::ComboBox midiOutputCombo;
    juce::TextButton midiLearnButton;
    juce::TextButton midiClockButton;
    juce::TextButton midiRecordButton;
    juce::TextButton midiPlayButton;
    juce::TextButton loadMidiFileButton;
    juce::Label midiInputLabel;
    juce::Label midiOutputLabel;
    juce::Label midiLearnLabel;
    juce::Label midiClockLabel;
    juce::Label midiRecordLabel;
    juce::Label midiPlayLabel;
    juce::Label bpmLabel;
    juce::Label clockCountLabel;

    // State
    bool midiLearnEnabled = false;
    bool midiClockEnabled = false;
    bool midiRecording = false;
    bool midiPlaying = false;

    // Callbacks
    std::function<void(bool)> midiLearnCallback;
    std::function<void(bool)> midiClockCallback;

    // Event handlers
    void midiInputChanged();
    void midiOutputChanged();
    void midiLearnClicked();
    void midiClockClicked();
    void midiRecordClicked();
    void midiPlayClicked();
    void loadMidiFileClicked();

    // Helper methods
    void updateButtonStates();
    void updateLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSettingsPanel)
}; 