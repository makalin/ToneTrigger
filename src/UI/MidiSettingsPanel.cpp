#include "MidiSettingsPanel.h"
#include "../MidiProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

MidiSettingsPanel::MidiSettingsPanel()
{
    // Setup labels
    midiInputLabel.setText("MIDI Input:", juce::dontSendNotification);
    midiOutputLabel.setText("MIDI Output:", juce::dontSendNotification);
    midiLearnLabel.setText("MIDI Learn:", juce::dontSendNotification);
    midiClockLabel.setText("MIDI Clock:", juce::dontSendNotification);
    midiRecordLabel.setText("MIDI Record:", juce::dontSendNotification);
    midiPlayLabel.setText("MIDI Play:", juce::dontSendNotification);
    bpmLabel.setText("BPM: 120", juce::dontSendNotification);
    clockCountLabel.setText("Clock: 0", juce::dontSendNotification);

    // Setup buttons
    midiLearnButton.setButtonText("Enable Learn");
    midiClockButton.setButtonText("Enable Clock");
    midiRecordButton.setButtonText("Start Recording");
    midiPlayButton.setButtonText("Play MIDI");
    loadMidiFileButton.setButtonText("Load MIDI File");

    // Add components
    addAndMakeVisible(midiInputLabel);
    addAndMakeVisible(midiOutputLabel);
    addAndMakeVisible(midiLearnLabel);
    addAndMakeVisible(midiClockLabel);
    addAndMakeVisible(midiRecordLabel);
    addAndMakeVisible(midiPlayLabel);
    addAndMakeVisible(bpmLabel);
    addAndMakeVisible(clockCountLabel);

    addAndMakeVisible(midiInputCombo);
    addAndMakeVisible(midiOutputCombo);
    addAndMakeVisible(midiLearnButton);
    addAndMakeVisible(midiClockButton);
    addAndMakeVisible(midiRecordButton);
    addAndMakeVisible(midiPlayButton);
    addAndMakeVisible(loadMidiFileButton);

    // Setup callbacks
    midiInputCombo.onChange = [this]() { midiInputChanged(); };
    midiOutputCombo.onChange = [this]() { midiOutputChanged(); };
    midiLearnButton.onClick = [this]() { midiLearnClicked(); };
    midiClockButton.onClick = [this]() { midiClockClicked(); };
    midiRecordButton.onClick = [this]() { midiRecordClicked(); };
    midiPlayButton.onClick = [this]() { midiPlayClicked(); };
    loadMidiFileButton.onClick = [this]() { loadMidiFileClicked(); };

    // Update device lists
    updateDeviceLists();
}

MidiSettingsPanel::~MidiSettingsPanel()
{
}

void MidiSettingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
}

void MidiSettingsPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    int labelWidth = 100;
    int comboWidth = 200;
    int buttonWidth = 120;
    int rowHeight = 30;
    int spacing = 10;

    // First row - MIDI Input
    auto row1 = bounds.removeFromTop(rowHeight);
    midiInputLabel.setBounds(row1.removeFromLeft(labelWidth));
    row1.removeFromLeft(spacing);
    midiInputCombo.setBounds(row1.removeFromLeft(comboWidth));

    bounds.removeFromTop(spacing);

    // Second row - MIDI Output
    auto row2 = bounds.removeFromTop(rowHeight);
    midiOutputLabel.setBounds(row2.removeFromLeft(labelWidth));
    row2.removeFromLeft(spacing);
    midiOutputCombo.setBounds(row2.removeFromLeft(comboWidth));

    bounds.removeFromTop(spacing);

    // Third row - MIDI Learn and Clock
    auto row3 = bounds.removeFromTop(rowHeight);
    midiLearnLabel.setBounds(row3.removeFromLeft(labelWidth));
    row3.removeFromLeft(spacing);
    midiLearnButton.setBounds(row3.removeFromLeft(buttonWidth));
    row3.removeFromLeft(spacing);
    midiClockLabel.setBounds(row3.removeFromLeft(labelWidth));
    row3.removeFromLeft(spacing);
    midiClockButton.setBounds(row3.removeFromLeft(buttonWidth));

    bounds.removeFromTop(spacing);

    // Fourth row - MIDI Record and Play
    auto row4 = bounds.removeFromTop(rowHeight);
    midiRecordLabel.setBounds(row4.removeFromLeft(labelWidth));
    row4.removeFromLeft(spacing);
    midiRecordButton.setBounds(row4.removeFromLeft(buttonWidth));
    row4.removeFromLeft(spacing);
    midiPlayLabel.setBounds(row4.removeFromLeft(labelWidth));
    row4.removeFromLeft(spacing);
    midiPlayButton.setBounds(row4.removeFromLeft(buttonWidth));

    bounds.removeFromTop(spacing);

    // Fifth row - Load MIDI File
    auto row5 = bounds.removeFromTop(rowHeight);
    loadMidiFileButton.setBounds(row5.removeFromLeft(buttonWidth));

    bounds.removeFromTop(spacing);

    // Sixth row - Status labels
    auto row6 = bounds.removeFromTop(rowHeight);
    bpmLabel.setBounds(row6.removeFromLeft(100));
    row6.removeFromLeft(spacing);
    clockCountLabel.setBounds(row6.removeFromLeft(100));
}

void MidiSettingsPanel::setMidiProcessor(MidiProcessor* processor)
{
    midiProcessor = processor;
    updateDeviceLists();
}

void MidiSettingsPanel::updateDeviceLists()
{
    if (!midiProcessor)
        return;

    // Update MIDI input devices
    midiInputCombo.clear();
    midiInputCombo.addItem("None", -1);
    
    auto inputDevices = midiProcessor->getMidiInputDevices();
    for (int i = 0; i < inputDevices.size(); ++i)
    {
        midiInputCombo.addItem(inputDevices[i], i + 1);
    }

    // Update MIDI output devices
    midiOutputCombo.clear();
    midiOutputCombo.addItem("None", -1);
    
    auto outputDevices = midiProcessor->getMidiOutputDevices();
    for (int i = 0; i < outputDevices.size(); ++i)
    {
        midiOutputCombo.addItem(outputDevices[i], i + 1);
    }

    updateButtonStates();
}

void MidiSettingsPanel::setMidiLearnCallback(std::function<void(bool)> callback)
{
    midiLearnCallback = callback;
}

void MidiSettingsPanel::setMidiClockCallback(std::function<void(bool)> callback)
{
    midiClockCallback = callback;
}

void MidiSettingsPanel::midiInputChanged()
{
    if (!midiProcessor)
        return;

    int selectedId = midiInputCombo.getSelectedId();
    if (selectedId > 0)
    {
        auto devices = midiProcessor->getMidiInputDevices();
        if (selectedId - 1 < devices.size())
        {
            midiProcessor->setMidiInputDevice(devices[selectedId - 1]);
        }
    }
    else
    {
        midiProcessor->setMidiInputDevice("");
    }
}

void MidiSettingsPanel::midiOutputChanged()
{
    if (!midiProcessor)
        return;

    int selectedId = midiOutputCombo.getSelectedId();
    if (selectedId > 0)
    {
        auto devices = midiProcessor->getMidiOutputDevices();
        if (selectedId - 1 < devices.size())
        {
            midiProcessor->setMidiOutputDevice(devices[selectedId - 1]);
        }
    }
    else
    {
        midiProcessor->setMidiOutputDevice("");
    }
}

void MidiSettingsPanel::midiLearnClicked()
{
    if (!midiProcessor)
        return;

    midiLearnEnabled = !midiLearnEnabled;
    midiProcessor->enableMidiLearn(midiLearnEnabled);
    
    if (midiLearnEnabled)
    {
        midiLearnButton.setButtonText("Disable Learn");
        midiLearnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        midiLearnButton.setButtonText("Enable Learn");
        midiLearnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }

    if (midiLearnCallback)
        midiLearnCallback(midiLearnEnabled);
}

void MidiSettingsPanel::midiClockClicked()
{
    if (!midiProcessor)
        return;

    midiClockEnabled = !midiClockEnabled;
    midiProcessor->setMidiClockEnabled(midiClockEnabled);
    
    if (midiClockEnabled)
    {
        midiClockButton.setButtonText("Disable Clock");
        midiClockButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
    }
    else
    {
        midiClockButton.setButtonText("Enable Clock");
        midiClockButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }

    if (midiClockCallback)
        midiClockCallback(midiClockEnabled);
}

void MidiSettingsPanel::midiRecordClicked()
{
    if (!midiProcessor)
        return;

    midiRecording = !midiRecording;
    
    if (midiRecording)
    {
        midiProcessor->startMidiRecording();
        midiRecordButton.setButtonText("Stop Recording");
        midiRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        midiProcessor->stopMidiRecording();
        midiRecordButton.setButtonText("Start Recording");
        midiRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }
}

void MidiSettingsPanel::midiPlayClicked()
{
    if (!midiProcessor)
        return;

    midiPlaying = !midiPlaying;
    
    if (midiPlaying)
    {
        midiProcessor->playMidiFile();
        midiPlayButton.setButtonText("Stop Playing");
        midiPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    }
    else
    {
        midiProcessor->stopMidiPlayback();
        midiPlayButton.setButtonText("Play MIDI");
        midiPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }
}

void MidiSettingsPanel::loadMidiFileClicked()
{
    if (!midiProcessor)
        return;

    juce::FileChooser chooser("Select MIDI File",
                              juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                              "*.mid;*.midi");

    if (chooser.browseForFileToOpen())
    {
        juce::File file = chooser.getResult();
        midiProcessor->loadMidiFile(file);
    }
}

void MidiSettingsPanel::updateButtonStates()
{
    // Update button states based on current state
    if (midiLearnEnabled)
    {
        midiLearnButton.setButtonText("Disable Learn");
        midiLearnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        midiLearnButton.setButtonText("Enable Learn");
        midiLearnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }

    if (midiClockEnabled)
    {
        midiClockButton.setButtonText("Disable Clock");
        midiClockButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
    }
    else
    {
        midiClockButton.setButtonText("Enable Clock");
        midiClockButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }

    if (midiRecording)
    {
        midiRecordButton.setButtonText("Stop Recording");
        midiRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        midiRecordButton.setButtonText("Start Recording");
        midiRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }

    if (midiPlaying)
    {
        midiPlayButton.setButtonText("Stop Playing");
        midiPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    }
    else
    {
        midiPlayButton.setButtonText("Play MIDI");
        midiPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }
}

void MidiSettingsPanel::updateLabels()
{
    if (midiProcessor)
    {
        double bpm = midiProcessor->getBPM();
        int clockCount = midiProcessor->getMidiClockCount();
        
        bpmLabel.setText("BPM: " + juce::String(bpm, 1), juce::dontSendNotification);
        clockCountLabel.setText("Clock: " + juce::String(clockCount), juce::dontSendNotification);
    }
} 