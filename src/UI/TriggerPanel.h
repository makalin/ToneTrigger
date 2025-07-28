#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class AudioProcessor;

class TriggerPanel : public juce::Component
{
public:
    explicit TriggerPanel(AudioProcessor* processor);
    ~TriggerPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    AudioProcessor* audioProcessor;

    // UI Components
    juce::TextButton addNoteTriggerButton;
    juce::TextButton addChordTriggerButton;
    juce::TextButton addMelodyTriggerButton;
    juce::TextButton removeTriggerButton;
    
    juce::ComboBox noteComboBox;
    juce::ComboBox effectComboBox;
    juce::Slider thresholdSlider;
    
    juce::ListBox triggerListBox;
    juce::Label triggerListLabel;
    
    // Trigger data
    struct TriggerItem
    {
        int id;
        juce::String name;
        juce::String type;
        juce::String effect;
        bool enabled;
    };
    
    std::vector<TriggerItem> triggerItems;
    
    // Methods
    void setupUI();
    void addNoteTrigger();
    void addChordTrigger();
    void addMelodyTrigger();
    void removeSelectedTrigger();
    void updateTriggerList();
    
    // Helper methods
    juce::String noteNumberToString(int note);
    int stringToNoteNumber(const juce::String& noteString);
    juce::String getEffectName(int effectId);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerPanel)
}; 