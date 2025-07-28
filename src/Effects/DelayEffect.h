#pragma once

#include "BaseEffect.h"
#include <vector>

class DelayEffect : public BaseEffect
{
public:
    DelayEffect();
    ~DelayEffect() override;

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
    juce::String getName() const override { return "Delay"; }
    juce::String getDescription() const override { return "Echo delay effect"; }

    // Parameter IDs
    enum Parameters
    {
        Time = 0,
        Feedback = 1,
        Mix = 2
    };

private:
    // Parameters
    float delayTime = 0.3f;  // seconds
    float feedback = 0.3f;
    float mix = 0.5f;

    // Delay buffer
    std::vector<float> delayBuffer;
    int delayBufferSize = 0;
    int writeIndex = 0;
    int readIndex = 0;

    // Processing
    void updateDelayBuffer();
    float processSample(float input);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayEffect)
}; 