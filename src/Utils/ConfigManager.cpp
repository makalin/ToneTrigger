#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
    loadConfiguration();
}

ConfigManager::~ConfigManager()
{
    saveConfiguration();
}

void ConfigManager::loadConfiguration()
{
    auto configFile = getConfigFile();
    if (!configFile.existsAsFile())
    {
        createDefaultConfiguration();
    }
}

void ConfigManager::saveConfiguration()
{
    auto configFile = getConfigFile();
    configFile.create();
}

void ConfigManager::resetToDefaults()
{
    createDefaultConfiguration();
}

void ConfigManager::setAudioDevice(const juce::String& inputDevice, const juce::String& outputDevice)
{
    setSetting("InputDevice", inputDevice);
    setSetting("OutputDevice", outputDevice);
}

void ConfigManager::getAudioDevice(juce::String& inputDevice, juce::String& outputDevice)
{
    inputDevice = getSetting("InputDevice", "").toString();
    outputDevice = getSetting("OutputDevice", "").toString();
}

void ConfigManager::setSampleRate(double sampleRate)
{
    setSetting("SampleRate", sampleRate);
}

double ConfigManager::getSampleRate() const
{
    return getSetting("SampleRate", Defaults::defaultSampleRate);
}

void ConfigManager::setBufferSize(int bufferSize)
{
    setSetting("BufferSize", bufferSize);
}

int ConfigManager::getBufferSize() const
{
    return getSetting("BufferSize", Defaults::defaultBufferSize);
}

void ConfigManager::setEffectParameter(int effectId, int parameterId, float value)
{
    juce::String key = "Effect_" + juce::String(effectId) + "_Param_" + juce::String(parameterId);
    setSetting(key, value);
}

float ConfigManager::getEffectParameter(int effectId, int parameterId) const
{
    juce::String key = "Effect_" + juce::String(effectId) + "_Param_" + juce::String(parameterId);
    return getSetting(key, 0.5f);
}

void ConfigManager::setEffectEnabled(int effectId, bool enabled)
{
    juce::String key = "Effect_" + juce::String(effectId) + "_Enabled";
    setSetting(key, enabled);
}

bool ConfigManager::isEffectEnabled(int effectId) const
{
    juce::String key = "Effect_" + juce::String(effectId) + "_Enabled";
    return getSetting(key, true);
}

void ConfigManager::saveTrigger(int triggerId, const juce::String& type, const std::vector<int>& notes, int effectId)
{
    juce::String key = "Trigger_" + juce::String(triggerId);
    juce::var triggerData;
    triggerData.append(type);
    triggerData.append(effectId);
    
    juce::var notesArray;
    for (int note : notes)
    {
        notesArray.append(note);
    }
    triggerData.append(notesArray);
    
    setSetting(key, triggerData);
}

void ConfigManager::loadTriggers(std::vector<juce::var>& triggers)
{
    triggers.clear();
    
    // Load all trigger settings
    auto configFile = getConfigFile();
    if (configFile.existsAsFile())
    {
        juce::PropertiesFile props(configFile, juce::PropertiesFile::Options());
        
        for (int i = 0; i < props.getAllProperties().size(); ++i)
        {
            juce::String key = props.getAllProperties().getAllKeys()[i];
            if (key.startsWith("Trigger_"))
            {
                triggers.push_back(props.getValue(key));
            }
        }
    }
}

void ConfigManager::clearTriggers()
{
    auto configFile = getConfigFile();
    if (configFile.existsAsFile())
    {
        juce::PropertiesFile props(configFile, juce::PropertiesFile::Options());
        
        for (int i = props.getAllProperties().size() - 1; i >= 0; --i)
        {
            juce::String key = props.getAllProperties().getAllKeys()[i];
            if (key.startsWith("Trigger_"))
            {
                props.removeValue(key);
            }
        }
        
        props.save();
    }
}

void ConfigManager::savePreset(const juce::String& presetName)
{
    auto presetsDir = getPresetsDirectory();
    if (!presetsDir.exists())
        presetsDir.createDirectory();
    
    auto presetFile = presetsDir.getChildFile(presetName + ".preset");
    juce::PropertiesFile props(presetFile, juce::PropertiesFile::Options());
    
    // Save current configuration to preset
    auto configFile = getConfigFile();
    if (configFile.existsAsFile())
    {
        juce::PropertiesFile configProps(configFile, juce::PropertiesFile::Options());
        
        for (int i = 0; i < configProps.getAllProperties().size(); ++i)
        {
            juce::String key = configProps.getAllProperties().getAllKeys()[i];
            juce::var value = configProps.getValue(key);
            props.setValue(key, value);
        }
    }
    
    props.save();
}

void ConfigManager::loadPreset(const juce::String& presetName)
{
    auto presetsDir = getPresetsDirectory();
    auto presetFile = presetsDir.getChildFile(presetName + ".preset");
    
    if (presetFile.existsAsFile())
    {
        juce::PropertiesFile presetProps(presetFile, juce::PropertiesFile::Options());
        auto configFile = getConfigFile();
        juce::PropertiesFile configProps(configFile, juce::PropertiesFile::Options());
        
        // Load preset configuration
        for (int i = 0; i < presetProps.getAllProperties().size(); ++i)
        {
            juce::String key = presetProps.getAllProperties().getAllKeys()[i];
            juce::var value = presetProps.getValue(key);
            configProps.setValue(key, value);
        }
        
        configProps.save();
    }
}

void ConfigManager::deletePreset(const juce::String& presetName)
{
    auto presetsDir = getPresetsDirectory();
    auto presetFile = presetsDir.getChildFile(presetName + ".preset");
    
    if (presetFile.existsAsFile())
    {
        presetFile.deleteFile();
    }
}

juce::StringArray ConfigManager::getPresetNames() const
{
    juce::StringArray presetNames;
    auto presetsDir = getPresetsDirectory();
    
    if (presetsDir.exists())
    {
        auto files = presetsDir.findChildFiles(juce::File::findFiles, false, "*.preset");
        for (auto& file : files)
        {
            presetNames.add(file.getFileNameWithoutExtension());
        }
    }
    
    return presetNames;
}

void ConfigManager::setSetting(const juce::String& key, const juce::var& value)
{
    auto configFile = getConfigFile();
    juce::PropertiesFile props(configFile, juce::PropertiesFile::Options());
    props.setValue(key, value);
    props.save();
}

juce::var ConfigManager::getSetting(const juce::String& key, const juce::var& defaultValue) const
{
    auto configFile = getConfigFile();
    if (configFile.existsAsFile())
    {
        juce::PropertiesFile props(configFile, juce::PropertiesFile::Options());
        return props.getValue(key, defaultValue);
    }
    return defaultValue;
}

void ConfigManager::createDefaultConfiguration()
{
    setSampleRate(Defaults::defaultSampleRate);
    setBufferSize(Defaults::defaultBufferSize);
    setSetting("InputGain", Defaults::defaultInputGain);
    setSetting("OutputGain", Defaults::defaultOutputGain);
    setSetting("WindowWidth", 1200);
    setSetting("WindowHeight", 800);
}

juce::File ConfigManager::getConfigFile()
{
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto configDir = appDataDir.getChildFile("ToneTrigger");
    
    if (!configDir.exists())
        configDir.createDirectory();
    
    return configDir.getChildFile("config.properties");
}

juce::File ConfigManager::getPresetsDirectory()
{
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto configDir = appDataDir.getChildFile("ToneTrigger");
    return configDir.getChildFile("Presets");
} 