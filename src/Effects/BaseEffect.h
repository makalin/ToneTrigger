#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class BaseEffect
{
public:
    BaseEffect() = default;
    virtual ~BaseEffect() = default;

    // Setup
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) = 0;
    virtual void releaseResources() = 0;

    // Audio processing
    virtual void processAudio(juce::AudioBuffer<float>& buffer) = 0;

    // Parameter management
    virtual void setParameter(int parameterId, float value) = 0;
    virtual float getParameter(int parameterId) const = 0;
    virtual int getNumParameters() const = 0;
    virtual juce::String getParameterName(int parameterId) const = 0;
    virtual float getParameterDefaultValue(int parameterId) const = 0;
    virtual float getParameterMinValue(int parameterId) const = 0;
    virtual float getParameterMaxValue(int parameterId) const = 0;

    // Effect info
    virtual juce::String getName() const = 0;
    virtual juce::String getDescription() const = 0;

protected:
    double sampleRate = 44100.0;
    int blockSize = 256;
}; 