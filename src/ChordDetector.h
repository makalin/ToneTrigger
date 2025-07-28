#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <string>
#include <map>

struct ChordInfo
{
    std::string name;
    std::vector<int> intervals;
    float confidence;
    int rootNote;
    
    ChordInfo(const std::string& chordName, const std::vector<int>& chordIntervals, 
              float conf, int root)
        : name(chordName), intervals(chordIntervals), confidence(conf), rootNote(root) {}
};

class ChordDetector
{
public:
    ChordDetector();
    ~ChordDetector();

    // Setup
    void prepareToPlay(double sampleRate);
    void releaseResources();

    // Chord detection
    ChordInfo detectChord(const std::vector<float>& frequencies, const std::vector<float>& magnitudes);
    std::vector<ChordInfo> detectAllChords(const std::vector<float>& frequencies, const std::vector<float>& magnitudes);
    
    // Analysis
    void analyzeHarmonics(const std::vector<float>& frequencies, const std::vector<float>& magnitudes);
    std::vector<int> extractNotes(const std::vector<float>& frequencies, const std::vector<float>& magnitudes);
    float calculateChordConfidence(const std::vector<int>& detectedNotes, const std::vector<int>& chordIntervals);

    // Chord database
    void addCustomChord(const std::string& name, const std::vector<int>& intervals);
    void removeCustomChord(const std::string& name);
    std::vector<std::string> getAvailableChords() const;

    // Settings
    void setDetectionThreshold(float threshold);
    void setHarmonicWeight(float weight);
    void setConfidenceThreshold(float threshold);
    void enableExtendedChords(bool enabled);

private:
    // Analysis parameters
    double sampleRate = 44100.0;
    float detectionThreshold = 0.1f;
    float harmonicWeight = 0.3f;
    float confidenceThreshold = 0.5f;
    bool extendedChordsEnabled = true;

    // Harmonic analysis
    std::vector<float> harmonicFrequencies;
    std::vector<float> harmonicMagnitudes;
    std::vector<int> detectedNotes;

    // Chord database
    std::map<std::string, std::vector<int>> chordDatabase;
    
    // Predefined chords
    void initializeChordDatabase();
    
    // Helper methods
    float frequencyToNote(float frequency);
    int noteToMidi(float note);
    float midiToFrequency(int midiNote);
    std::vector<int> getNoteIntervals(const std::vector<int>& notes);
    bool intervalsMatch(const std::vector<int>& intervals1, const std::vector<int>& intervals2, float tolerance = 1.0f);
    float calculateNoteStrength(float frequency, float magnitude);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordDetector)
}; 