#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <vector>
#include <map>

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();

    // Configuration management
    void loadConfiguration();
    void saveConfiguration();
    void resetToDefaults();

    // Audio settings
    void setAudioDevice(const juce::String& inputDevice, const juce::String& outputDevice);
    void getAudioDevice(juce::String& inputDevice, juce::String& outputDevice);
    void setSampleRate(double sampleRate);
    double getSampleRate() const;
    void setBufferSize(int bufferSize);
    int getBufferSize() const;

    // Effect settings
    void setEffectParameter(int effectId, int parameterId, float value);
    float getEffectParameter(int effectId, int parameterId) const;
    void setEffectEnabled(int effectId, bool enabled);
    bool isEffectEnabled(int effectId) const;

    // Trigger settings
    void saveTrigger(int triggerId, const juce::String& type, const std::vector<int>& notes, int effectId);
    void loadTriggers(std::vector<juce::var>& triggers);
    void clearTriggers();

    // Preset management
    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    juce::StringArray getPresetNames() const;

    // General settings
    void setSetting(const juce::String& key, const juce::var& value);
    juce::var getSetting(const juce::String& key, const juce::var& defaultValue = juce::var()) const;

private:
    juce::PropertiesFile configFile;
    juce::File configDirectory;
    
    // Default values
    struct Defaults
    {
        static constexpr double defaultSampleRate = 44100.0;
        static constexpr int defaultBufferSize = 256;
        static constexpr float defaultInputGain = 1.0f;
        static constexpr float defaultOutputGain = 1.0f;
    };

    // Helper methods
    void createDefaultConfiguration();
    juce::File getConfigFile();
    juce::File getPresetsDirectory();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigManager)
}; 