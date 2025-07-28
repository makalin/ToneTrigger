#pragma once

#include "BaseEffect.h"
#include <vector>

enum class FilterType
{
    LowPass,
    HighPass,
    BandPass,
    Notch
};

class FilterEffect : public BaseEffect
{
public:
    FilterEffect();
    ~FilterEffect() override;

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
    juce::String getName() const override { return "Filter"; }
    juce::String getDescription() const override { return "Multi-mode filter effect"; }

    // Parameter IDs
    enum Parameters
    {
        Cutoff = 0,
        Resonance = 1,
        FilterType = 2,
        Drive = 3
    };

private:
    // Parameters
    float cutoffFreq = 1000.0f;
    float resonance = 0.5f;
    FilterType filterType = FilterType::LowPass;
    float drive = 0.0f;

    // Filter state
    std::vector<float> x1, x2, y1, y2; // Filter history for each channel
    float a0, a1, a2, b1, b2; // Filter coefficients

    // Processing
    void updateFilterCoefficients();
    float processSample(float input, int channel);
    void calculateLowPassCoefficients();
    void calculateHighPassCoefficients();
    void calculateBandPassCoefficients();
    void calculateNotchCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterEffect)
}; 