#include "AudioAnalyzer.h"
#include <cmath>
#include <algorithm>

AudioAnalyzer::AudioAnalyzer()
{
}

AudioAnalyzer::~AudioAnalyzer()
{
}

void AudioAnalyzer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
    
    // Initialize analysis buffer
    analysisBuffer.resize(analysisWindowSize);
    std::fill(analysisBuffer.begin(), analysisBuffer.end(), 0.0f);
    
    // Initialize history buffers
    noteHistory.clear();
    amplitudeHistory.clear();
}

void AudioAnalyzer::releaseResources()
{
    analysisBuffer.clear();
    noteHistory.clear();
    amplitudeHistory.clear();
}

void AudioAnalyzer::processAudio(const juce::AudioBuffer<float>& buffer)
{
    analyzeAmplitude(buffer);
    
    // Only analyze if there's sufficient amplitude
    if (currentAmplitude > noteThreshold)
    {
        analyzeNote(buffer);
        analyzeChord(buffer);
        analyzeMelody(buffer);
    }
    else
    {
        // Reset detections if no signal
        currentNote = -1.0f;
        currentChord = -1.0f;
        currentMelody = -1.0f;
    }
}

void AudioAnalyzer::setNoteDetectionThreshold(float threshold)
{
    noteThreshold = juce::jlimit(0.0f, 1.0f, threshold);
}

void AudioAnalyzer::setChordDetectionThreshold(float threshold)
{
    chordThreshold = juce::jlimit(0.0f, 1.0f, threshold);
}

void AudioAnalyzer::setMelodyDetectionThreshold(float threshold)
{
    melodyThreshold = juce::jlimit(0.0f, 1.0f, threshold);
}

void AudioAnalyzer::setAnalysisWindowSize(int windowSize)
{
    analysisWindowSize = juce::jlimit(256, 8192, windowSize);
    analysisBuffer.resize(analysisWindowSize);
}

void AudioAnalyzer::analyzeNote(const juce::AudioBuffer<float>& buffer)
{
    // Simple pitch detection using autocorrelation
    // In a real implementation, you'd want more sophisticated algorithms
    
    // Convert buffer to mono for analysis
    std::vector<float> monoBuffer(buffer.getNumSamples());
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float sum = 0.0f;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            sum += buffer.getSample(channel, sample);
        }
        monoBuffer[sample] = sum / buffer.getNumChannels();
    }
    
    float detectedPitch = detectPitch(monoBuffer);
    
    if (detectedPitch > 0)
    {
        currentNote = detectedPitch;
        updateHistory(noteHistory, detectedPitch, 10);
    }
}

void AudioAnalyzer::analyzeChord(const juce::AudioBuffer<float>& buffer)
{
    // Simple chord detection based on harmonic content
    // In a real implementation, you'd want more sophisticated chord recognition
    
    std::vector<float> monoBuffer(buffer.getNumSamples());
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float sum = 0.0f;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            sum += buffer.getSample(channel, sample);
        }
        monoBuffer[sample] = sum / buffer.getNumChannels();
    }
    
    auto harmonics = detectHarmonics(monoBuffer);
    
    if (!harmonics.empty())
    {
        // Use the strongest harmonic as chord root
        currentChord = harmonics[0];
    }
}

void AudioAnalyzer::analyzeMelody(const juce::AudioBuffer<float>& buffer)
{
    // Simple melody detection based on note history
    // In a real implementation, you'd want more sophisticated melody tracking
    
    if (!noteHistory.empty())
    {
        // Use the most recent note as current melody
        currentMelody = noteHistory.back();
    }
}

void AudioAnalyzer::analyzeAmplitude(const juce::AudioBuffer<float>& buffer)
{
    currentAmplitude = calculateRMS(buffer);
    updateHistory(amplitudeHistory, currentAmplitude, 20);
}

float AudioAnalyzer::detectPitch(const std::vector<float>& buffer)
{
    // Simple autocorrelation-based pitch detection
    // This is a basic implementation - real applications would use more sophisticated algorithms
    
    if (buffer.size() < 512)
        return -1.0f;
    
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
        
        // Convert frequency to MIDI note number
        if (frequency > 20.0f && frequency < 20000.0f)
        {
            return 12.0f * std::log2(frequency / 440.0f) + 69.0f;
        }
    }
    
    return -1.0f;
}

std::vector<float> AudioAnalyzer::detectHarmonics(const std::vector<float>& buffer)
{
    // Simple harmonic detection using FFT
    // In a real implementation, you'd use JUCE's FFT class
    
    std::vector<float> harmonics;
    
    // For now, just return the fundamental frequency
    float fundamental = detectPitch(buffer);
    if (fundamental > 0)
    {
        harmonics.push_back(fundamental);
    }
    
    return harmonics;
}

float AudioAnalyzer::calculateRMS(const juce::AudioBuffer<float>& buffer)
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

void AudioAnalyzer::updateHistory(std::deque<float>& history, float value, int maxSize)
{
    history.push_back(value);
    if (history.size() > maxSize)
    {
        history.pop_front();
    }
} 