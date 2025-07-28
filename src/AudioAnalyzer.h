#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <deque>

class AudioAnalyzer
{
public:
    AudioAnalyzer();
    ~AudioAnalyzer();

    // Setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources();

    // Audio processing
    void processAudio(const juce::AudioBuffer<float>& buffer);

    // Analysis results
    float getCurrentNote() const { return currentNote; }
    float getCurrentChord() const { return currentChord; }
    float getCurrentMelody() const { return currentMelody; }
    float getCurrentAmplitude() const { return currentAmplitude; }

    // Configuration
    void setNoteDetectionThreshold(float threshold);
    void setChordDetectionThreshold(float threshold);
    void setMelodyDetectionThreshold(float threshold);
    void setAnalysisWindowSize(int windowSize);

private:
    // Audio parameters
    double sampleRate = 44100.0;
    int blockSize = 256;
    int analysisWindowSize = 2048;

    // Detection thresholds
    float noteThreshold = 0.1f;
    float chordThreshold = 0.15f;
    float melodyThreshold = 0.1f;

    // Analysis results
    float currentNote = -1.0f;
    float currentChord = -1.0f;
    float currentMelody = -1.0f;
    float currentAmplitude = 0.0f;

    // Analysis buffers
    std::vector<float> analysisBuffer;
    std::deque<float> noteHistory;
    std::deque<float> amplitudeHistory;

    // Analysis methods
    void analyzeNote(const juce::AudioBuffer<float>& buffer);
    void analyzeChord(const juce::AudioBuffer<float>& buffer);
    void analyzeMelody(const juce::AudioBuffer<float>& buffer);
    void analyzeAmplitude(const juce::AudioBuffer<float>& buffer);
    
    // Helper methods
    float detectPitch(const std::vector<float>& buffer);
    std::vector<float> detectHarmonics(const std::vector<float>& buffer);
    float calculateRMS(const juce::AudioBuffer<float>& buffer);
    void updateHistory(std::deque<float>& history, float value, int maxSize);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioAnalyzer)
}; 