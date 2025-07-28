#pragma once

#include "BaseEffect.h"

class CompressorEffect : public BaseEffect
{
public:
    CompressorEffect();
    ~CompressorEffect() override;

    // Setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    // Audio processing
    void processAudio(juce::AudioBuffer<float>& buffer) override;

    // Parameter management
    void setParameter(int parameterId, float value) override;
    float getParameter(int parameterId) const override;
    int getNumParameters() const override { return 5; }
    juce::String getParameterName(int parameterId) const override;
    float getParameterDefaultValue(int parameterId) const override;
    float getParameterMinValue(int parameterId) const override;
    float getParameterMaxValue(int parameterId) const override;

    // Effect info
    juce::String getName() const override { return "Compressor"; }
    juce::String getDescription() const override { return "Dynamic range compressor"; }

    // Parameter IDs
    enum Parameters
    {
        Threshold = 0,
        Ratio = 1,
        Attack = 2,
        Release = 3,
        MakeupGain = 4
    };

private:
    // Parameters
    float threshold = -20.0f; // dB
    float ratio = 4.0f;
    float attack = 10.0f; // ms
    float release = 100.0f; // ms
    float makeupGain = 0.0f; // dB

    // Compressor state
    float envelope = 0.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float makeupGainLinear = 1.0f;

    // Processing
    void updateCoefficients();
    float processSample(float input, int channel);
    float calculateGainReduction(float inputLevel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorEffect)
}; 