#include "EffectProcessor.h"
#include "Effects/DistortionEffect.h"
#include "Effects/ReverbEffect.h"
#include "Effects/DelayEffect.h"
#include "Effects/ChorusEffect.h"
#include "Effects/FilterEffect.h"
#include "Effects/CompressorEffect.h"

EffectProcessor::EffectProcessor()
{
}

EffectProcessor::~EffectProcessor()
{
}

void EffectProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;

    // Prepare all effects
    for (auto& effectInstance : effects)
    {
        if (effectInstance.effect)
            effectInstance.effect->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void EffectProcessor::releaseResources()
{
    for (auto& effectInstance : effects)
    {
        if (effectInstance.effect)
            effectInstance.effect->releaseResources();
    }
}

int EffectProcessor::addEffect(EffectType type)
{
    auto effect = createEffect(type);
    if (effect)
    {
        EffectInstance instance(nextEffectId++, type, std::move(effect));
        effects.push_back(std::move(instance));
        
        // Set as active if it's the first effect
        if (activeEffectId == -1)
            activeEffectId = instance.id;
            
        return instance.id;
    }
    return -1;
}

void EffectProcessor::removeEffect(int effectId)
{
    auto it = std::find_if(effects.begin(), effects.end(),
                          [effectId](const EffectInstance& e) { return e.id == effectId; });
    if (it != effects.end())
    {
        if (activeEffectId == effectId)
            activeEffectId = -1;
        effects.erase(it);
    }
}

void EffectProcessor::setEffectEnabled(int effectId, bool enabled)
{
    auto it = std::find_if(effects.begin(), effects.end(),
                          [effectId](const EffectInstance& e) { return e.id == effectId; });
    if (it != effects.end())
    {
        it->enabled = enabled;
    }
}

bool EffectProcessor::isEffectEnabled(int effectId) const
{
    auto it = std::find_if(effects.begin(), effects.end(),
                          [effectId](const EffectInstance& e) { return e.id == effectId; });
    return it != effects.end() && it->enabled;
}

void EffectProcessor::setParameter(int effectId, int parameterId, float value)
{
    auto it = std::find_if(effects.begin(), effects.end(),
                          [effectId](const EffectInstance& e) { return e.id == effectId; });
    if (it != effects.end())
    {
        it->parameters[parameterId] = value;
        if (it->effect)
            it->effect->setParameter(parameterId, value);
    }
}

float EffectProcessor::getParameter(int effectId, int parameterId) const
{
    auto it = std::find_if(effects.begin(), effects.end(),
                          [effectId](const EffectInstance& e) { return e.id == effectId; });
    if (it != effects.end())
    {
        auto paramIt = it->parameters.find(parameterId);
        if (paramIt != it->parameters.end())
            return paramIt->second;
    }
    return 0.0f;
}

void EffectProcessor::setActiveEffect(int effectId)
{
    activeEffectId = effectId;
}

void EffectProcessor::processAudio(juce::AudioBuffer<float>& buffer)
{
    // Process only the active effect
    if (activeEffectId != -1)
    {
        auto it = std::find_if(effects.begin(), effects.end(),
                              [this](const EffectInstance& e) { return e.id == activeEffectId; });
        if (it != effects.end() && it->enabled && it->effect)
        {
            it->effect->processAudio(buffer);
        }
    }
}

EffectInstance* EffectProcessor::getEffect(int effectId)
{
    auto it = std::find_if(effects.begin(), effects.end(),
                          [effectId](const EffectInstance& e) { return e.id == effectId; });
    return it != effects.end() ? &(*it) : nullptr;
}

std::unique_ptr<BaseEffect> EffectProcessor::createEffect(EffectType type)
{
    switch (type)
    {
        case EffectType::Distortion:
            return std::make_unique<DistortionEffect>();
        case EffectType::Reverb:
            return std::make_unique<ReverbEffect>();
        case EffectType::Delay:
            return std::make_unique<DelayEffect>();
        case EffectType::Chorus:
            return std::make_unique<ChorusEffect>();
        case EffectType::Filter:
            return std::make_unique<FilterEffect>();
        case EffectType::Compressor:
            return std::make_unique<CompressorEffect>();
        default:
            return nullptr;
    }
} 