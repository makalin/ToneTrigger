#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

namespace AudioUtils
{
    // Frequency and pitch utilities
    float frequencyToMidiNote(float frequency);
    float midiNoteToFrequency(float midiNote);
    juce::String midiNoteToString(int midiNote);
    int stringToMidiNote(const juce::String& noteString);
    
    // Audio analysis utilities
    float calculateRMS(const juce::AudioBuffer<float>& buffer);
    float calculatePeak(const juce::AudioBuffer<float>& buffer);
    float calculateDB(float amplitude);
    float dbToAmplitude(float db);
    
    // Filter utilities
    float calculateLowPassCoefficient(float cutoffFreq, double sampleRate);
    float calculateHighPassCoefficient(float cutoffFreq, double sampleRate);
    
    // Delay utilities
    int timeToSamples(float timeMs, double sampleRate);
    float samplesToTime(int samples, double sampleRate);
    
    // Math utilities
    float clamp(float value, float min, float max);
    float linearToDecibels(float linear);
    float decibelsToLinear(float db);
    
    // Buffer utilities
    void copyBuffer(const juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& destination);
    void clearBuffer(juce::AudioBuffer<float>& buffer);
    void applyGain(juce::AudioBuffer<float>& buffer, float gain);
    
    // Window functions
    std::vector<float> createHannWindow(int size);
    std::vector<float> createHammingWindow(int size);
    std::vector<float> createBlackmanWindow(int size);
    
    // FFT utilities (basic implementation)
    void applyWindow(std::vector<float>& buffer, const std::vector<float>& window);
    float findFundamentalFrequency(const std::vector<float>& buffer, double sampleRate);
}; 