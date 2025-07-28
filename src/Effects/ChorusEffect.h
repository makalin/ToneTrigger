#pragma once

#include "BaseEffect.h"
#include <vector>

class ChorusEffect : public BaseEffect
{
public:
    ChorusEffect();
    ~ChorusEffect() override;

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
    juce::String getName() const override { return "Chorus"; }
    juce::String getDescription() const override { return "Chorus modulation effect"; }

    // Parameter IDs
    enum Parameters
    {
        Rate = 0,
        Depth = 1,
        Mix = 2
    };

private:
    // Parameters
    float rate = 1.0f;    // Hz
    float depth = 0.5f;   // 0-1
    float mix = 0.5f;     // 0-1

    // LFO state
    float lfoPhase = 0.0f;
    float lfoIncrement = 0.0f;

    // Delay buffer
    std::vector<float> delayBuffer;
    int delayBufferSize = 0;
    int writeIndex = 0;

    // Processing
    void updateLFO();
    float getLFOValue();
    float processSample(float input);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusEffect)
}; 