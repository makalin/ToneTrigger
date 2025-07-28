#include "ChorusEffect.h"
#include <cmath>

ChorusEffect::ChorusEffect()
{
}

ChorusEffect::~ChorusEffect()
{
}

void ChorusEffect::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
    
    // Initialize delay buffer (max 50ms delay)
    delayBufferSize = static_cast<int>(0.05f * sampleRate);
    delayBuffer.resize(delayBufferSize);
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
    writeIndex = 0;
    
    updateLFO();
}

void ChorusEffect::releaseResources()
{
    delayBuffer.clear();
    delayBufferSize = 0;
    writeIndex = 0;
    lfoPhase = 0.0f;
}

void ChorusEffect::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float input = channelData[sample];
            float processed = processSample(input);
            
            // Mix dry and wet signals
            channelData[sample] = input * (1.0f - mix) + processed * mix;
            
            // Update LFO
            updateLFO();
        }
    }
}

void ChorusEffect::setParameter(int parameterId, float value)
{
    switch (parameterId)
    {
        case Rate:
            rate = juce::jlimit(0.1f, 10.0f, value);
            updateLFO();
            break;
        case Depth:
            depth = juce::jlimit(0.0f, 1.0f, value);
            break;
        case Mix:
            mix = juce::jlimit(0.0f, 1.0f, value);
            break;
    }
}

float ChorusEffect::getParameter(int parameterId) const
{
    switch (parameterId)
    {
        case Rate: return rate;
        case Depth: return depth;
        case Mix: return mix;
        default: return 0.0f;
    }
}

juce::String ChorusEffect::getParameterName(int parameterId) const
{
    switch (parameterId)
    {
        case Rate: return "Rate";
        case Depth: return "Depth";
        case Mix: return "Mix";
        default: return "Unknown";
    }
}

float ChorusEffect::getParameterDefaultValue(int parameterId) const
{
    switch (parameterId)
    {
        case Rate: return 1.0f;
        case Depth: return 0.5f;
        case Mix: return 0.5f;
        default: return 0.0f;
    }
}

float ChorusEffect::getParameterMinValue(int parameterId) const
{
    switch (parameterId)
    {
        case Rate: return 0.1f;
        case Depth: return 0.0f;
        case Mix: return 0.0f;
        default: return 0.0f;
    }
}

float ChorusEffect::getParameterMaxValue(int parameterId) const
{
    switch (parameterId)
    {
        case Rate: return 10.0f;
        case Depth: return 1.0f;
        case Mix: return 1.0f;
        default: return 1.0f;
    }
}

void ChorusEffect::updateLFO()
{
    lfoIncrement = 2.0f * M_PI * rate / sampleRate;
    lfoPhase += lfoIncrement;
    
    // Keep phase in range [0, 2π]
    while (lfoPhase >= 2.0f * M_PI)
        lfoPhase -= 2.0f * M_PI;
}

float ChorusEffect::getLFOValue()
{
    // Sine wave LFO
    return std::sin(lfoPhase);
}

float ChorusEffect::processSample(float input)
{
    if (delayBufferSize == 0)
        return input;
    
    // Write to delay buffer
    delayBuffer[writeIndex] = input;
    
    // Calculate modulated delay time
    float lfoValue = getLFOValue();
    float delayOffset = lfoValue * depth * delayBufferSize * 0.5f;
    float readIndex = writeIndex - delayOffset;
    
    // Handle wrap-around
    while (readIndex < 0)
        readIndex += delayBufferSize;
    while (readIndex >= delayBufferSize)
        readIndex -= delayBufferSize;
    
    // Linear interpolation
    int readIndexInt = static_cast<int>(readIndex);
    float fraction = readIndex - readIndexInt;
    int nextIndex = (readIndexInt + 1) % delayBufferSize;
    
    float delayed1 = delayBuffer[readIndexInt];
    float delayed2 = delayBuffer[nextIndex];
    float delayed = delayed1 + fraction * (delayed2 - delayed1);
    
    // Update write index
    writeIndex = (writeIndex + 1) % delayBufferSize;
    
    return delayed;
} 