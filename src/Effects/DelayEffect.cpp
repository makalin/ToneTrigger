#include "DelayEffect.h"
#include <cmath>

DelayEffect::DelayEffect()
{
}

DelayEffect::~DelayEffect()
{
}

void DelayEffect::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
    
    updateDelayBuffer();
}

void DelayEffect::releaseResources()
{
    delayBuffer.clear();
    delayBufferSize = 0;
    writeIndex = 0;
    readIndex = 0;
}

void DelayEffect::processAudio(juce::AudioBuffer<float>& buffer)
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
        }
    }
}

void DelayEffect::setParameter(int parameterId, float value)
{
    switch (parameterId)
    {
        case Time:
            delayTime = juce::jlimit(0.01f, 2.0f, value);
            updateDelayBuffer();
            break;
        case Feedback:
            feedback = juce::jlimit(0.0f, 0.9f, value);
            break;
        case Mix:
            mix = juce::jlimit(0.0f, 1.0f, value);
            break;
    }
}

float DelayEffect::getParameter(int parameterId) const
{
    switch (parameterId)
    {
        case Time: return delayTime;
        case Feedback: return feedback;
        case Mix: return mix;
        default: return 0.0f;
    }
}

juce::String DelayEffect::getParameterName(int parameterId) const
{
    switch (parameterId)
    {
        case Time: return "Time";
        case Feedback: return "Feedback";
        case Mix: return "Mix";
        default: return "Unknown";
    }
}

float DelayEffect::getParameterDefaultValue(int parameterId) const
{
    switch (parameterId)
    {
        case Time: return 0.3f;
        case Feedback: return 0.3f;
        case Mix: return 0.5f;
        default: return 0.0f;
    }
}

float DelayEffect::getParameterMinValue(int parameterId) const
{
    switch (parameterId)
    {
        case Time: return 0.01f;
        case Feedback: return 0.0f;
        case Mix: return 0.0f;
        default: return 0.0f;
    }
}

float DelayEffect::getParameterMaxValue(int parameterId) const
{
    switch (parameterId)
    {
        case Time: return 2.0f;
        case Feedback: return 0.9f;
        case Mix: return 1.0f;
        default: return 1.0f;
    }
}

void DelayEffect::updateDelayBuffer()
{
    // Calculate delay buffer size
    int delaySamples = static_cast<int>(delayTime * sampleRate);
    delayBufferSize = delaySamples;
    
    // Resize buffer
    delayBuffer.resize(delayBufferSize);
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
    
    // Reset indices
    writeIndex = 0;
    readIndex = 0;
}

float DelayEffect::processSample(float input)
{
    if (delayBufferSize == 0)
        return input;
    
    // Read delayed sample
    float delayed = delayBuffer[readIndex];
    
    // Calculate output (input + feedback * delayed)
    float output = input + feedback * delayed;
    
    // Write to delay buffer
    delayBuffer[writeIndex] = output;
    
    // Update indices
    writeIndex = (writeIndex + 1) % delayBufferSize;
    readIndex = (readIndex + 1) % delayBufferSize;
    
    return output;
} 