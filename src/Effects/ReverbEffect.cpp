#include "ReverbEffect.h"
#include <cmath>

ReverbEffect::ReverbEffect()
{
    // Initialize delay line lengths (prime numbers for better diffusion)
    delayLengths = { 1116, 1188, 1277, 1356, 1422, 1492, 1557, 1617 };
}

ReverbEffect::~ReverbEffect()
{
}

void ReverbEffect::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
    
    initializeDelayLines();
}

void ReverbEffect::releaseResources()
{
    delayLines.clear();
    delayLineIndices.clear();
    delayLineCoeffs.clear();
}

void ReverbEffect::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float input = channelData[sample];
            float processed = processSample(input, channel);
            
            // Mix dry and wet signals
            channelData[sample] = input * dryLevel + processed * wetLevel;
        }
    }
}

void ReverbEffect::setParameter(int parameterId, float value)
{
    switch (parameterId)
    {
        case RoomSize:
            roomSize = juce::jlimit(0.0f, 1.0f, value);
            break;
        case Damping:
            damping = juce::jlimit(0.0f, 1.0f, value);
            break;
        case WetLevel:
            wetLevel = juce::jlimit(0.0f, 1.0f, value);
            break;
        case DryLevel:
            dryLevel = juce::jlimit(0.0f, 1.0f, value);
            break;
    }
}

float ReverbEffect::getParameter(int parameterId) const
{
    switch (parameterId)
    {
        case RoomSize: return roomSize;
        case Damping: return damping;
        case WetLevel: return wetLevel;
        case DryLevel: return dryLevel;
        default: return 0.0f;
    }
}

juce::String ReverbEffect::getParameterName(int parameterId) const
{
    switch (parameterId)
    {
        case RoomSize: return "Room Size";
        case Damping: return "Damping";
        case WetLevel: return "Wet Level";
        case DryLevel: return "Dry Level";
        default: return "Unknown";
    }
}

float ReverbEffect::getParameterDefaultValue(int parameterId) const
{
    switch (parameterId)
    {
        case RoomSize: return 0.5f;
        case Damping: return 0.5f;
        case WetLevel: return 0.3f;
        case DryLevel: return 0.7f;
        default: return 0.0f;
    }
}

float ReverbEffect::getParameterMinValue(int parameterId) const
{
    return 0.0f;
}

float ReverbEffect::getParameterMaxValue(int parameterId) const
{
    return 1.0f;
}

void ReverbEffect::initializeDelayLines()
{
    int numDelayLines = delayLengths.size();
    delayLines.resize(numDelayLines);
    delayLineIndices.resize(numDelayLines);
    delayLineCoeffs.resize(numDelayLines);
    
    // Scale delay lengths based on room size
    float sizeMultiplier = 0.5f + roomSize * 2.0f;
    
    for (int i = 0; i < numDelayLines; ++i)
    {
        int scaledLength = static_cast<int>(delayLengths[i] * sizeMultiplier);
        delayLines[i].resize(scaledLength);
        std::fill(delayLines[i].begin(), delayLines[i].end(), 0.0f);
        delayLineIndices[i] = 0;
        
        // Set feedback coefficient based on damping
        delayLineCoeffs[i] = 0.6f - damping * 0.3f; // 0.3 to 0.6
    }
}

float ReverbEffect::processSample(float input, int channel)
{
    float output = 0.0f;
    
    // Process through all delay lines
    for (size_t i = 0; i < delayLines.size(); ++i)
    {
        // Get delayed sample
        float delayed = delayLines[i][delayLineIndices[i]];
        
        // Add to output
        output += delayed;
        
        // Update delay line
        float feedback = input + delayed * delayLineCoeffs[i];
        delayLines[i][delayLineIndices[i]] = feedback;
        
        // Update index
        delayLineIndices[i] = (delayLineIndices[i] + 1) % delayLines[i].size();
    }
    
    // Normalize output
    output /= static_cast<float>(delayLines.size());
    
    return output;
} 