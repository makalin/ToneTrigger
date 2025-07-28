#include "TriggerPanel.h"
#include "../AudioProcessor.h"
#include "../TriggerManager.h"

TriggerPanel::TriggerPanel(AudioProcessor* processor)
    : audioProcessor(processor)
{
    setupUI();
}

TriggerPanel::~TriggerPanel()
{
}

void TriggerPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2d2d2d));
}

void TriggerPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Top controls area
    auto topArea = bounds.removeFromTop(120);
    
    // Left side - trigger type buttons
    auto leftArea = topArea.removeFromLeft(200);
    addNoteTriggerButton.setBounds(leftArea.removeFromTop(30).reduced(5));
    addChordTriggerButton.setBounds(leftArea.removeFromTop(30).reduced(5));
    addMelodyTriggerButton.setBounds(leftArea.removeFromTop(30).reduced(5));
    removeTriggerButton.setBounds(leftArea.removeFromTop(30).reduced(5));
    
    // Right side - parameter controls
    auto rightArea = topArea;
    noteComboBox.setBounds(rightArea.removeFromTop(25).reduced(5));
    effectComboBox.setBounds(rightArea.removeFromTop(25).reduced(5));
    thresholdSlider.setBounds(rightArea.removeFromTop(25).reduced(5));
    
    // Trigger list
    triggerListLabel.setBounds(bounds.removeFromTop(25));
    triggerListBox.setBounds(bounds);
}

void TriggerPanel::setupUI()
{
    // Setup buttons
    addNoteTriggerButton.setButtonText("Add Note Trigger");
    addNoteTriggerButton.onClick = [this]() { addNoteTrigger(); };
    addAndMakeVisible(addNoteTriggerButton);
    
    addChordTriggerButton.setButtonText("Add Chord Trigger");
    addChordTriggerButton.onClick = [this]() { addChordTrigger(); };
    addAndMakeVisible(addChordTriggerButton);
    
    addMelodyTriggerButton.setButtonText("Add Melody Trigger");
    addMelodyTriggerButton.onClick = [this]() { addMelodyTrigger(); };
    addAndMakeVisible(addMelodyTriggerButton);
    
    removeTriggerButton.setButtonText("Remove Selected");
    removeTriggerButton.onClick = [this]() { removeSelectedTrigger(); };
    addAndMakeVisible(removeTriggerButton);
    
    // Setup note combo box
    noteComboBox.addItem("C", 60);
    noteComboBox.addItem("C#", 61);
    noteComboBox.addItem("D", 62);
    noteComboBox.addItem("D#", 63);
    noteComboBox.addItem("E", 64);
    noteComboBox.addItem("F", 65);
    noteComboBox.addItem("F#", 66);
    noteComboBox.addItem("G", 67);
    noteComboBox.addItem("G#", 68);
    noteComboBox.addItem("A", 69);
    noteComboBox.addItem("A#", 70);
    noteComboBox.addItem("B", 71);
    noteComboBox.setSelectedId(60);
    addAndMakeVisible(noteComboBox);
    
    // Setup effect combo box
    effectComboBox.addItem("Distortion", 1);
    effectComboBox.addItem("Reverb", 2);
    effectComboBox.addItem("Delay", 3);
    effectComboBox.addItem("Chorus", 4);
    effectComboBox.setSelectedId(1);
    addAndMakeVisible(effectComboBox);
    
    // Setup threshold slider
    thresholdSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    thresholdSlider.setRange(0.0, 1.0, 0.01);
    thresholdSlider.setValue(0.5);
    addAndMakeVisible(thresholdSlider);
    
    // Setup trigger list
    triggerListLabel.setText("Active Triggers:", juce::dontSendNotification);
    triggerListLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(triggerListLabel);
    
    triggerListBox.setModel(this);
    addAndMakeVisible(triggerListBox);
}

void TriggerPanel::addNoteTrigger()
{
    if (!audioProcessor)
        return;
    
    int note = noteComboBox.getSelectedId();
    int effectId = effectComboBox.getSelectedId();
    float threshold = static_cast<float>(thresholdSlider.getValue());
    
    audioProcessor->addNoteTrigger(note, effectId);
    updateTriggerList();
}

void TriggerPanel::addChordTrigger()
{
    if (!audioProcessor)
        return;
    
    // For simplicity, we'll create a major chord based on the selected note
    int rootNote = noteComboBox.getSelectedId();
    std::vector<int> chordNotes = {rootNote, rootNote + 4, rootNote + 7}; // Major triad
    int effectId = effectComboBox.getSelectedId();
    
    audioProcessor->addChordTrigger(chordNotes, effectId);
    updateTriggerList();
}

void TriggerPanel::addMelodyTrigger()
{
    if (!audioProcessor)
        return;
    
    // For simplicity, we'll create a simple melody based on the selected note
    int rootNote = noteComboBox.getSelectedId();
    std::vector<int> melodyNotes = {rootNote, rootNote + 2, rootNote + 4, rootNote + 7}; // Simple scale
    int effectId = effectComboBox.getSelectedId();
    
    audioProcessor->addMelodyTrigger(melodyNotes, effectId);
    updateTriggerList();
}

void TriggerPanel::removeSelectedTrigger()
{
    int selectedRow = triggerListBox.getSelectedRow();
    if (selectedRow >= 0 && selectedRow < triggerItems.size())
    {
        int triggerId = triggerItems[selectedRow].id;
        audioProcessor->removeTrigger(triggerId);
        updateTriggerList();
    }
}

void TriggerPanel::updateTriggerList()
{
    if (!audioProcessor)
        return;
    
    triggerItems.clear();
    
    // Get triggers from the trigger manager
    auto triggerManager = audioProcessor->getTriggerManager();
    if (triggerManager)
    {
        const auto& triggers = triggerManager->getTriggers();
        for (const auto& trigger : triggers)
        {
            TriggerItem item;
            item.id = trigger.id;
            item.enabled = trigger.enabled;
            
            switch (trigger.type)
            {
                case TriggerType::Note:
                    item.type = "Note";
                    item.name = noteNumberToString(trigger.notes[0]);
                    break;
                case TriggerType::Chord:
                    item.type = "Chord";
                    item.name = noteNumberToString(trigger.notes[0]) + " Major";
                    break;
                case TriggerType::Melody:
                    item.type = "Melody";
                    item.name = "Sequence (" + juce::String(trigger.notes.size()) + " notes)";
                    break;
            }
            
            item.effect = getEffectName(trigger.effectId);
            triggerItems.push_back(item);
        }
    }
    
    triggerListBox.updateContent();
}

juce::String TriggerPanel::noteNumberToString(int note)
{
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (note / 12) - 1;
    int noteInOctave = note % 12;
    return juce::String(noteNames[noteInOctave]) + juce::String(octave);
}

int TriggerPanel::stringToNoteNumber(const juce::String& noteString)
{
    // Simple conversion - in a real app you'd want more robust parsing
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    for (int i = 0; i < 12; ++i)
    {
        if (noteString.startsWith(noteNames[i]))
        {
            int octave = noteString.substring(juce::String(noteNames[i]).length()).getIntValue();
            return (octave + 1) * 12 + i;
        }
    }
    
    return 60; // Default to middle C
}

juce::String TriggerPanel::getEffectName(int effectId)
{
    switch (effectId)
    {
        case 1: return "Distortion";
        case 2: return "Reverb";
        case 3: return "Delay";
        case 4: return "Chorus";
        default: return "Unknown";
    }
} 