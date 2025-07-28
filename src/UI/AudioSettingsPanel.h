#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

class AudioSettingsPanel : public juce::Component
{
public:
    explicit AudioSettingsPanel(juce::AudioDeviceManager* deviceManager);
    ~AudioSettingsPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioDeviceManager* deviceManager;

    // UI Components
    juce::ComboBox inputDeviceComboBox;
    juce::ComboBox outputDeviceComboBox;
    juce::ComboBox sampleRateComboBox;
    juce::ComboBox bufferSizeComboBox;
    
    juce::Label inputDeviceLabel;
    juce::Label outputDeviceLabel;
    juce::Label sampleRateLabel;
    juce::Label bufferSizeLabel;
    
    juce::TextButton testButton;
    juce::Label statusLabel;

    // Methods
    void setupUI();
    void updateDeviceLists();
    void updateSampleRates();
    void updateBufferSizes();
    void onDeviceChanged();
    void onSampleRateChanged();
    void onBufferSizeChanged();
    void testAudio();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsPanel)
}; 