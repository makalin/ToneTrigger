#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <vector>
#include <map>
#include <functional>

struct EffectPreset
{
    juce::String name;
    juce::String effectType;
    std::map<int, float> parameters;
    bool enabled;
    
    EffectPreset() : enabled(true) {}
};

struct TriggerPreset
{
    juce::String name;
    juce::String triggerType;
    std::vector<int> notes;
    int effectId;
    float threshold;
    int duration;
    bool enabled;
    
    TriggerPreset() : effectId(-1), threshold(0.5f), duration(100), enabled(true) {}
};

struct AudioPreset
{
    juce::String name;
    juce::String inputDevice;
    juce::String outputDevice;
    double sampleRate;
    int bufferSize;
    float inputGain;
    float outputGain;
    
    AudioPreset() : sampleRate(44100.0), bufferSize(256), inputGain(1.0f), outputGain(1.0f) {}
};

struct CompletePreset
{
    juce::String name;
    juce::String description;
    juce::String author;
    juce::String version;
    juce::Time creationTime;
    
    std::vector<EffectPreset> effects;
    std::vector<TriggerPreset> triggers;
    AudioPreset audioSettings;
    
    CompletePreset() : version("1.0.0") {}
};

class PresetManager
{
public:
    PresetManager();
    ~PresetManager();

    // Preset management
    void savePreset(const CompletePreset& preset);
    CompletePreset loadPreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    juce::StringArray getPresetNames() const;
    bool presetExists(const juce::String& presetName) const;

    // Individual preset types
    void saveEffectPreset(const juce::String& name, const EffectPreset& preset);
    EffectPreset loadEffectPreset(const juce::String& name);
    juce::StringArray getEffectPresetNames() const;

    void saveTriggerPreset(const juce::String& name, const TriggerPreset& preset);
    TriggerPreset loadTriggerPreset(const juce::String& name);
    juce::StringArray getTriggerPresetNames() const;

    void saveAudioPreset(const juce::String& name, const AudioPreset& preset);
    AudioPreset loadAudioPreset(const juce::String& name);
    juce::StringArray getAudioPresetNames() const;

    // Preset categories
    void createPresetCategory(const juce::String& categoryName);
    void deletePresetCategory(const juce::String& categoryName);
    juce::StringArray getPresetCategories() const;
    juce::StringArray getPresetsInCategory(const juce::String& categoryName) const;

    // Preset validation
    bool validatePreset(const CompletePreset& preset) const;
    juce::String getPresetValidationError(const CompletePreset& preset) const;

    // Preset import/export
    void exportPreset(const juce::String& presetName, const juce::File& file);
    void importPreset(const juce::File& file);
    void exportAllPresets(const juce::File& directory);
    void importAllPresets(const juce::File& directory);

    // Preset templates
    void createPresetTemplate(const juce::String& templateName, const CompletePreset& templatePreset);
    CompletePreset loadPresetTemplate(const juce::String& templateName);
    juce::StringArray getPresetTemplateNames() const;

    // Preset callbacks
    void setPresetChangedCallback(std::function<void(const juce::String&)> callback);
    void setPresetSavedCallback(std::function<void(const juce::String&)> callback);
    void setPresetLoadedCallback(std::function<void(const juce::String&)> callback);

    // Utility methods
    juce::String generatePresetName(const juce::String& baseName) const;
    void backupPresets();
    void restorePresets();
    void clearAllPresets();

private:
    juce::File presetsDirectory;
    juce::File templatesDirectory;
    juce::File backupDirectory;

    // Callbacks
    std::function<void(const juce::String&)> presetChangedCallback;
    std::function<void(const juce::String&)> presetSavedCallback;
    std::function<void(const juce::String&)> presetLoadedCallback;

    // Helper methods
    void ensureDirectoriesExist();
    juce::File getPresetFile(const juce::String& presetName) const;
    juce::File getEffectPresetFile(const juce::String& presetName) const;
    juce::File getTriggerPresetFile(const juce::String& presetName) const;
    juce::File getAudioPresetFile(const juce::String& presetName) const;
    juce::File getTemplateFile(const juce::String& templateName) const;
    
    CompletePreset loadPresetFromFile(const juce::File& file);
    void savePresetToFile(const CompletePreset& preset, const juce::File& file);
    
    juce::var presetToVar(const CompletePreset& preset) const;
    CompletePreset varToPreset(const juce::var& data) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
}; 