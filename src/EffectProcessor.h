#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <vector>
#include <map>

class BaseEffect;

enum class EffectType
{
    Distortion,
    Reverb,
    Delay,
    Chorus,
    Filter,
    Compressor
};

struct EffectInstance
{
    int id;
    EffectType type;
    std::unique_ptr<BaseEffect> effect;
    bool enabled;
    std::map<int, float> parameters;
    
    EffectInstance(int effectId, EffectType effectType, std::unique_ptr<BaseEffect> effectPtr)
        : id(effectId), type(effectType), effect(std::move(effectPtr)), enabled(true) {}
};

class EffectProcessor
{
public:
    EffectProcessor();
    ~EffectProcessor();

    // Setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources();

    // Effect management
    int addEffect(EffectType type);
    void removeEffect(int effectId);
    void setEffectEnabled(int effectId, bool enabled);
    bool isEffectEnabled(int effectId) const;

    // Parameter management
    void setParameter(int effectId, int parameterId, float value);
    float getParameter(int effectId, int parameterId) const;
    void setActiveEffect(int effectId);
    int getActiveEffect() const { return activeEffectId; }

    // Audio processing
    void processAudio(juce::AudioBuffer<float>& buffer);

    // Getters
    const std::vector<EffectInstance>& getEffects() const { return effects; }
    EffectInstance* getEffect(int effectId);

private:
    // Effects
    std::vector<EffectInstance> effects;
    int nextEffectId = 1;
    int activeEffectId = -1;

    // Audio parameters
    double sampleRate = 44100.0;
    int blockSize = 256;

    // Helper methods
    std::unique_ptr<BaseEffect> createEffect(EffectType type);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectProcessor)
}; 