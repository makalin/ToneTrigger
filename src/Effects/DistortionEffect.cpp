#include "DistortionEffect.h"
#include <cmath>

DistortionEffect::DistortionEffect()
{
}

DistortionEffect::~DistortionEffect()
{
}

void DistortionEffect::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
}

void DistortionEffect::releaseResources()
{
}

void DistortionEffect::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = processSample(channelData[sample]);
        }
    }
}

void DistortionEffect::setParameter(int parameterId, float value)
{
    switch (parameterId)
    {
        case Drive:
            drive = juce::jlimit(0.0f, 1.0f, value);
            break;
        case Tone:
            tone = juce::jlimit(0.0f, 1.0f, value);
            break;
        case Level:
            level = juce::jlimit(0.0f, 1.0f, value);
            break;
    }
}

float DistortionEffect::getParameter(int parameterId) const
{
    switch (parameterId)
    {
        case Drive: return drive;
        case Tone: return tone;
        case Level: return level;
        default: return 0.0f;
    }
}

juce::String DistortionEffect::getParameterName(int parameterId) const
{
    switch (parameterId)
    {
        case Drive: return "Drive";
        case Tone: return "Tone";
        case Level: return "Level";
        default: return "Unknown";
    }
}

float DistortionEffect::getParameterDefaultValue(int parameterId) const
{
    switch (parameterId)
    {
        case Drive: return 0.5f;
        case Tone: return 0.5f;
        case Level: return 0.5f;
        default: return 0.0f;
    }
}

float DistortionEffect::getParameterMinValue(int parameterId) const
{
    return 0.0f;
}

float DistortionEffect::getParameterMaxValue(int parameterId) const
{
    return 1.0f;
}

float DistortionEffect::processSample(float input)
{
    // Apply drive
    float distorted = applyDrive(input);
    
    // Apply tone filter
    float filtered = applyToneFilter(distorted);
    
    // Apply level
    return filtered * level;
}

float DistortionEffect::applyDrive(float input)
{
    // Soft clipping distortion
    float driveAmount = 1.0f + drive * 10.0f; // 1x to 11x gain
    float driven = input * driveAmount;
    
    // Soft clipping using tanh approximation
    return std::tanh(driven);
}

float DistortionEffect::applyToneFilter(float input)
{
    // Simple low-pass filter based on tone parameter
    // Higher tone values = brighter sound (less filtering)
    float cutoffFreq = 200.0f + tone * 8000.0f; // 200Hz to 8.2kHz
    float rc = 1.0f / (2.0f * M_PI * cutoffFreq);
    float dt = 1.0f / sampleRate;
    float alpha = dt / (rc + dt);
    
    // Simple first-order low-pass filter
    static float lastOutput = 0.0f;
    float output = alpha * input + (1.0f - alpha) * lastOutput;
    lastOutput = output;
    
    return output;
} 