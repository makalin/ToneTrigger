#pragma once

#include "BaseEffect.h"
#include <vector>

class ReverbEffect : public BaseEffect
{
public:
    ReverbEffect();
    ~ReverbEffect() override;

    // Setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    // Audio processing
    void processAudio(juce::AudioBuffer<float>& buffer) override;

    // Parameter management
    void setParameter(int parameterId, float value) override;
    float getParameter(int parameterId) const override;
    int getNumParameters() const override { return 4; }
    juce::String getParameterName(int parameterId) const override;
    float getParameterDefaultValue(int parameterId) const override;
    float getParameterMinValue(int parameterId) const override;
    float getParameterMaxValue(int parameterId) const override;

    // Effect info
    juce::String getName() const override { return "Reverb"; }
    juce::String getDescription() const override { return "Room reverb effect"; }

    // Parameter IDs
    enum Parameters
    {
        RoomSize = 0,
        Damping = 1,
        WetLevel = 2,
        DryLevel = 3
    };

private:
    // Parameters
    float roomSize = 0.5f;
    float damping = 0.5f;
    float wetLevel = 0.3f;
    float dryLevel = 0.7f;

    // Reverb buffers
    std::vector<std::vector<float>> delayLines;
    std::vector<int> delayLineIndices;
    std::vector<float> delayLineCoeffs;
    
    // Delay line lengths (in samples)
    std::vector<int> delayLengths;

    // Processing
    void initializeDelayLines();
    float processSample(float input, int channel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbEffect)
}; 