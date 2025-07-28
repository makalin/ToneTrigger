#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "AudioProcessor.h"
#include "UI/TriggerPanel.h"
#include "UI/EffectPanel.h"
#include "UI/AudioSettingsPanel.h"

class MainComponent : public juce::Component,
                     public juce::AudioDeviceManager::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // AudioDeviceManager::Listener
    void audioDeviceListChanged() override;
    void audioDeviceSetupChanged() override;

private:
    // Audio components
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;
    std::unique_ptr<juce::AudioSourcePlayer> audioSourcePlayer;
    std::unique_ptr<AudioProcessor> audioProcessor;
    
    // UI components
    std::unique_ptr<TriggerPanel> triggerPanel;
    std::unique_ptr<EffectPanel> effectPanel;
    std::unique_ptr<AudioSettingsPanel> audioSettingsPanel;
    
    // UI elements
    juce::TabbedComponent tabbedComponent;
    juce::TextButton startStopButton;
    juce::Label statusLabel;
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    
    // State
    bool isPlaying = false;
    
    // Methods
    void setupAudio();
    void setupUI();
    void startAudio();
    void stopAudio();
    void updateStatus();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
}; 