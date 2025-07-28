#include "FilterEffect.h"
#include <cmath>

FilterEffect::FilterEffect()
{
}

FilterEffect::~FilterEffect()
{
}

void FilterEffect::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;

    // Initialize filter history for each channel
    x1.resize(2, 0.0f);
    x2.resize(2, 0.0f);
    y1.resize(2, 0.0f);
    y2.resize(2, 0.0f);

    updateFilterCoefficients();
}

void FilterEffect::releaseResources()
{
    x1.clear();
    x2.clear();
    y1.clear();
    y2.clear();
}

void FilterEffect::processAudio(juce::AudioBuffer<float>& buffer)
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

void FilterEffect::setParameter(int parameterId, float value)
{
    switch (parameterId)
    {
        case Cutoff:
            cutoffFreq = juce::jlimit(20.0f, 20000.0f, value);
            updateFilterCoefficients();
            break;
        case Resonance:
            resonance = juce::jlimit(0.0f, 1.0f, value);
            updateFilterCoefficients();
            break;
        case FilterType:
            filterType = static_cast<FilterType>(static_cast<int>(value * 3.0f));
            updateFilterCoefficients();
            break;
        case Drive:
            drive = juce::jlimit(0.0f, 1.0f, value);
            break;
    }
}

float FilterEffect::getParameter(int parameterId) const
{
    switch (parameterId)
    {
        case Cutoff: return cutoffFreq;
        case Resonance: return resonance;
        case FilterType: return static_cast<float>(filterType) / 3.0f;
        case Drive: return drive;
        default: return 0.0f;
    }
}

juce::String FilterEffect::getParameterName(int parameterId) const
{
    switch (parameterId)
    {
        case Cutoff: return "Cutoff";
        case Resonance: return "Resonance";
        case FilterType: return "Filter Type";
        case Drive: return "Drive";
        default: return "Unknown";
    }
}

float FilterEffect::getParameterDefaultValue(int parameterId) const
{
    switch (parameterId)
    {
        case Cutoff: return 1000.0f;
        case Resonance: return 0.5f;
        case FilterType: return 0.0f; // LowPass
        case Drive: return 0.0f;
        default: return 0.0f;
    }
}

float FilterEffect::getParameterMinValue(int parameterId) const
{
    switch (parameterId)
    {
        case Cutoff: return 20.0f;
        case Resonance: return 0.0f;
        case FilterType: return 0.0f;
        case Drive: return 0.0f;
        default: return 0.0f;
    }
}

float FilterEffect::getParameterMaxValue(int parameterId) const
{
    switch (parameterId)
    {
        case Cutoff: return 20000.0f;
        case Resonance: return 1.0f;
        case FilterType: return 1.0f;
        case Drive: return 1.0f;
        default: return 1.0f;
    }
}

void FilterEffect::updateFilterCoefficients()
{
    switch (filterType)
    {
        case FilterType::LowPass:
            calculateLowPassCoefficients();
            break;
        case FilterType::HighPass:
            calculateHighPassCoefficients();
            break;
        case FilterType::BandPass:
            calculateBandPassCoefficients();
            break;
        case FilterType::Notch:
            calculateNotchCoefficients();
            break;
    }
}

float FilterEffect::processSample(float input, int channel)
{
    // Apply drive
    if (drive > 0.0f)
    {
        float driveAmount = 1.0f + drive * 5.0f;
        input *= driveAmount;
        input = std::tanh(input);
    }

    // Apply filter
    float output = a0 * input + a1 * x1[channel] + a2 * x2[channel] - b1 * y1[channel] - b2 * y2[channel];

    // Update history
    x2[channel] = x1[channel];
    x1[channel] = input;
    y2[channel] = y1[channel];
    y1[channel] = output;

    return output;
}

void FilterEffect::calculateLowPassCoefficients()
{
    float w0 = 2.0f * M_PI * cutoffFreq / sampleRate;
    float alpha = std::sin(w0) * resonance;
    float cosw0 = std::cos(w0);

    float b0 = 1.0f + alpha;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha;
    float a0 = (1.0f - cosw0) / 2.0f;
    float a1 = 1.0f - cosw0;
    float a2 = (1.0f - cosw0) / 2.0f;

    // Normalize
    this->a0 = a0 / b0;
    this->a1 = a1 / b0;
    this->a2 = a2 / b0;
    this->b1 = b1 / b0;
    this->b2 = b2 / b0;
}

void FilterEffect::calculateHighPassCoefficients()
{
    float w0 = 2.0f * M_PI * cutoffFreq / sampleRate;
    float alpha = std::sin(w0) * resonance;
    float cosw0 = std::cos(w0);

    float b0 = 1.0f + alpha;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha;
    float a0 = (1.0f + cosw0) / 2.0f;
    float a1 = -(1.0f + cosw0);
    float a2 = (1.0f + cosw0) / 2.0f;

    // Normalize
    this->a0 = a0 / b0;
    this->a1 = a1 / b0;
    this->a2 = a2 / b0;
    this->b1 = b1 / b0;
    this->b2 = b2 / b0;
}

void FilterEffect::calculateBandPassCoefficients()
{
    float w0 = 2.0f * M_PI * cutoffFreq / sampleRate;
    float alpha = std::sin(w0) * resonance;
    float cosw0 = std::cos(w0);

    float b0 = 1.0f + alpha;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha;
    float a0 = alpha;
    float a1 = 0.0f;
    float a2 = -alpha;

    // Normalize
    this->a0 = a0 / b0;
    this->a1 = a1 / b0;
    this->a2 = a2 / b0;
    this->b1 = b1 / b0;
    this->b2 = b2 / b0;
}

void FilterEffect::calculateNotchCoefficients()
{
    float w0 = 2.0f * M_PI * cutoffFreq / sampleRate;
    float alpha = std::sin(w0) * resonance;
    float cosw0 = std::cos(w0);

    float b0 = 1.0f + alpha;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha;
    float a0 = 1.0f;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f;

    // Normalize
    this->a0 = a0 / b0;
    this->a1 = a1 / b0;
    this->a2 = a2 / b0;
    this->b1 = b1 / b0;
    this->b2 = b2 / b0;
} 