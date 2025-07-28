#include "CompressorEffect.h"
#include <cmath>

CompressorEffect::CompressorEffect()
{
}

CompressorEffect::~CompressorEffect()
{
}

void CompressorEffect::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
    
    updateCoefficients();
}

void CompressorEffect::releaseResources()
{
    envelope = 0.0f;
}

void CompressorEffect::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = processSample(channelData[sample], channel);
        }
    }
}

void CompressorEffect::setParameter(int parameterId, float value)
{
    switch (parameterId)
    {
        case Threshold:
            threshold = juce::jlimit(-60.0f, 0.0f, value);
            break;
        case Ratio:
            ratio = juce::jlimit(1.0f, 20.0f, value);
            break;
        case Attack:
            attack = juce::jlimit(0.1f, 100.0f, value);
            updateCoefficients();
            break;
        case Release:
            release = juce::jlimit(10.0f, 1000.0f, value);
            updateCoefficients();
            break;
        case MakeupGain:
            makeupGain = juce::jlimit(0.0f, 24.0f, value);
            makeupGainLinear = std::pow(10.0f, makeupGain / 20.0f);
            break;
    }
}

float CompressorEffect::getParameter(int parameterId) const
{
    switch (parameterId)
    {
        case Threshold: return threshold;
        case Ratio: return ratio;
        case Attack: return attack;
        case Release: return release;
        case MakeupGain: return makeupGain;
        default: return 0.0f;
    }
}

juce::String CompressorEffect::getParameterName(int parameterId) const
{
    switch (parameterId)
    {
        case Threshold: return "Threshold";
        case Ratio: return "Ratio";
        case Attack: return "Attack";
        case Release: return "Release";
        case MakeupGain: return "Makeup Gain";
        default: return "Unknown";
    }
}

float CompressorEffect::getParameterDefaultValue(int parameterId) const
{
    switch (parameterId)
    {
        case Threshold: return -20.0f;
        case Ratio: return 4.0f;
        case Attack: return 10.0f;
        case Release: return 100.0f;
        case MakeupGain: return 0.0f;
        default: return 0.0f;
    }
}

float CompressorEffect::getParameterMinValue(int parameterId) const
{
    switch (parameterId)
    {
        case Threshold: return -60.0f;
        case Ratio: return 1.0f;
        case Attack: return 0.1f;
        case Release: return 10.0f;
        case MakeupGain: return 0.0f;
        default: return 0.0f;
    }
}

float CompressorEffect::getParameterMaxValue(int parameterId) const
{
    switch (parameterId)
    {
        case Threshold: return 0.0f;
        case Ratio: return 20.0f;
        case Attack: return 100.0f;
        case Release: return 1000.0f;
        case MakeupGain: return 24.0f;
        default: return 1.0f;
    }
}

void CompressorEffect::updateCoefficients()
{
    // Convert ms to coefficients
    attackCoeff = std::exp(-1000.0f / (attack * sampleRate));
    releaseCoeff = std::exp(-1000.0f / (release * sampleRate));
}

float CompressorEffect::processSample(float input, int channel)
{
    // Calculate input level in dB
    float inputLevel = 20.0f * std::log10(std::abs(input) + 1e-6f);
    
    // Update envelope
    if (inputLevel > envelope)
    {
        envelope = attackCoeff * (envelope - inputLevel) + inputLevel;
    }
    else
    {
        envelope = releaseCoeff * (envelope - inputLevel) + inputLevel;
    }
    
    // Calculate gain reduction
    float gainReduction = calculateGainReduction(envelope);
    
    // Apply compression and makeup gain
    float output = input * gainReduction * makeupGainLinear;
    
    return output;
}

float CompressorEffect::calculateGainReduction(float inputLevel)
{
    if (inputLevel <= threshold)
    {
        return 1.0f; // No compression below threshold
    }
    
    // Calculate how much the signal exceeds the threshold
    float overThreshold = inputLevel - threshold;
    
    // Calculate compression amount
    float compressionAmount = overThreshold * (1.0f - 1.0f / ratio);
    
    // Convert to linear gain
    float gainReduction = std::pow(10.0f, -compressionAmount / 20.0f);
    
    return gainReduction;
} 