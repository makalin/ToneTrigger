#include "AudioUtils.h"
#include <cmath>
#include <algorithm>

namespace AudioUtils
{

float frequencyToMidiNote(float frequency)
{
    if (frequency <= 0.0f)
        return -1.0f;
    
    return 12.0f * std::log2(frequency / 440.0f) + 69.0f;
}

float midiNoteToFrequency(float midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

juce::String midiNoteToString(int midiNote)
{
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (midiNote / 12) - 1;
    int noteInOctave = midiNote % 12;
    return juce::String(noteNames[noteInOctave]) + juce::String(octave);
}

int stringToMidiNote(const juce::String& noteString)
{
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    for (int i = 0; i < 12; ++i)
    {
        if (noteString.startsWith(noteNames[i]))
        {
            int octave = noteString.substring(juce::String(noteNames[i]).length()).getIntValue();
            return (octave + 1) * 12 + i;
        }
    }
    
    return 60; // Default to middle C
}

float calculateRMS(const juce::AudioBuffer<float>& buffer)
{
    float sum = 0.0f;
    int totalSamples = buffer.getNumSamples() * buffer.getNumChannels();
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            sum += channelData[sample] * channelData[sample];
        }
    }
    
    return std::sqrt(sum / totalSamples);
}

float calculatePeak(const juce::AudioBuffer<float>& buffer)
{
    float peak = 0.0f;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            peak = std::max(peak, std::abs(channelData[sample]));
        }
    }
    
    return peak;
}

float calculateDB(float amplitude)
{
    if (amplitude <= 0.0f)
        return -96.0f; // Very low value for silence
    
    return 20.0f * std::log10(amplitude);
}

float dbToAmplitude(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

float calculateLowPassCoefficient(float cutoffFreq, double sampleRate)
{
    float rc = 1.0f / (2.0f * M_PI * cutoffFreq);
    float dt = 1.0f / sampleRate;
    return dt / (rc + dt);
}

float calculateHighPassCoefficient(float cutoffFreq, double sampleRate)
{
    float rc = 1.0f / (2.0f * M_PI * cutoffFreq);
    float dt = 1.0f / sampleRate;
    return rc / (rc + dt);
}

int timeToSamples(float timeMs, double sampleRate)
{
    return static_cast<int>(timeMs * sampleRate / 1000.0);
}

float samplesToTime(int samples, double sampleRate)
{
    return static_cast<float>(samples * 1000.0 / sampleRate);
}

float clamp(float value, float min, float max)
{
    return std::max(min, std::min(max, value));
}

float linearToDecibels(float linear)
{
    if (linear <= 0.0f)
        return -96.0f;
    
    return 20.0f * std::log10(linear);
}

float decibelsToLinear(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

void copyBuffer(const juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& destination)
{
    int numChannels = std::min(source.getNumChannels(), destination.getNumChannels());
    int numSamples = std::min(source.getNumSamples(), destination.getNumSamples());
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        destination.copyFrom(channel, 0, source, channel, 0, numSamples);
    }
}

void clearBuffer(juce::AudioBuffer<float>& buffer)
{
    buffer.clear();
}

void applyGain(juce::AudioBuffer<float>& buffer, float gain)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.applyGain(channel, 0, buffer.getNumSamples(), gain);
    }
}

std::vector<float> createHannWindow(int size)
{
    std::vector<float> window(size);
    for (int i = 0; i < size; ++i)
    {
        window[i] = 0.5f - 0.5f * std::cos(2.0f * M_PI * i / (size - 1));
    }
    return window;
}

std::vector<float> createHammingWindow(int size)
{
    std::vector<float> window(size);
    for (int i = 0; i < size; ++i)
    {
        window[i] = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (size - 1));
    }
    return window;
}

std::vector<float> createBlackmanWindow(int size)
{
    std::vector<float> window(size);
    for (int i = 0; i < size; ++i)
    {
        float x = 2.0f * M_PI * i / (size - 1);
        window[i] = 0.42f - 0.5f * std::cos(x) + 0.08f * std::cos(2.0f * x);
    }
    return window;
}

void applyWindow(std::vector<float>& buffer, const std::vector<float>& window)
{
    int size = std::min(static_cast<int>(buffer.size()), static_cast<int>(window.size()));
    for (int i = 0; i < size; ++i)
    {
        buffer[i] *= window[i];
    }
}

float findFundamentalFrequency(const std::vector<float>& buffer, double sampleRate)
{
    if (buffer.size() < 512)
        return -1.0f;
    
    // Simple autocorrelation-based pitch detection
    const int maxLag = std::min(static_cast<int>(buffer.size() / 2), 2048);
    std::vector<float> autocorr(maxLag);
    
    // Calculate autocorrelation
    for (int lag = 0; lag < maxLag; ++lag)
    {
        float sum = 0.0f;
        for (int i = 0; i < buffer.size() - lag; ++i)
        {
            sum += buffer[i] * buffer[i + lag];
        }
        autocorr[lag] = sum;
    }
    
    // Find the first peak after the initial peak
    float maxCorr = 0.0f;
    int maxIndex = 0;
    
    for (int i = 1; i < maxLag; ++i)
    {
        if (autocorr[i] > maxCorr)
        {
            maxCorr = autocorr[i];
            maxIndex = i;
        }
    }
    
    // Convert lag to frequency
    if (maxIndex > 0 && maxCorr > 0.1f)
    {
        float frequency = sampleRate / maxIndex;
        
        // Check if frequency is in reasonable range
        if (frequency > 20.0f && frequency < 20000.0f)
        {
            return frequency;
        }
    }
    
    return -1.0f;
}

} // namespace AudioUtils 