#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class AudioProcessor;

class EffectPanel : public juce::Component
{
public:
    explicit EffectPanel(AudioProcessor* processor);
    ~EffectPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    AudioProcessor* audioProcessor;

    // UI Components
    juce::TextButton addDistortionButton;
    juce::TextButton addReverbButton;
    juce::TextButton addDelayButton;
    juce::TextButton addChorusButton;
    juce::TextButton removeEffectButton;
    
    juce::ComboBox effectListComboBox;
    juce::ToggleButton effectEnabledToggle;
    
    juce::Label parameterLabel;
    std::vector<juce::Slider*> parameterSliders;
    std::vector<juce::Label*> parameterLabels;
    
    // Effect data
    struct EffectItem
    {
        int id;
        juce::String name;
        bool enabled;
    };
    
    std::vector<EffectItem> effectItems;
    
    // Methods
    void setupUI();
    void addEffect(int effectType);
    void removeSelectedEffect();
    void updateEffectList();
    void updateParameterControls();
    void onEffectSelected();
    void onParameterChanged(int parameterId, float value);
    
    // Helper methods
    juce::String getEffectTypeName(int effectType);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectPanel)
}; 