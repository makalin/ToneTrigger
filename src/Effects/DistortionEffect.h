#pragma once

#include "BaseEffect.h"

class DistortionEffect : public BaseEffect
{
public:
    DistortionEffect();
    ~DistortionEffect() override;

    // Setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    // Audio processing
    void processAudio(juce::AudioBuffer<float>& buffer) override;

    // Parameter management
    void setParameter(int parameterId, float value) override;
    float getParameter(int parameterId) const override;
    int getNumParameters() const override { return 3; }
    juce::String getParameterName(int parameterId) const override;
    float getParameterDefaultValue(int parameterId) const override;
    float getParameterMinValue(int parameterId) const override;
    float getParameterMaxValue(int parameterId) const override;

    // Effect info
    juce::String getName() const override { return "Distortion"; }
    juce::String getDescription() const override { return "Classic guitar distortion effect"; }

    // Parameter IDs
    enum Parameters
    {
        Drive = 0,
        Tone = 1,
        Level = 2
    };

private:
    // Parameters
    float drive = 0.5f;
    float tone = 0.5f;
    float level = 0.5f;

    // Processing
    float processSample(float input);
    float applyDrive(float input);
    float applyToneFilter(float input);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionEffect)
}; 