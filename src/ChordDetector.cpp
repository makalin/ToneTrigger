#include "ChordDetector.h"
#include <cmath>
#include <algorithm>
#include <numeric>

ChordDetector::ChordDetector()
{
    initializeChordDatabase();
}

ChordDetector::~ChordDetector()
{
}

void ChordDetector::prepareToPlay(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void ChordDetector::releaseResources()
{
    harmonicFrequencies.clear();
    harmonicMagnitudes.clear();
    detectedNotes.clear();
}

ChordInfo ChordDetector::detectChord(const std::vector<float>& frequencies, const std::vector<float>& magnitudes)
{
    analyzeHarmonics(frequencies, magnitudes);
    auto notes = extractNotes(frequencies, magnitudes);
    
    if (notes.empty())
        return ChordInfo("None", {}, 0.0f, -1);
    
    std::vector<ChordInfo> allChords = detectAllChords(frequencies, magnitudes);
    
    if (allChords.empty())
        return ChordInfo("Unknown", {}, 0.0f, notes[0]);
    
    // Return the chord with highest confidence
    auto bestChord = std::max_element(allChords.begin(), allChords.end(),
        [](const ChordInfo& a, const ChordInfo& b) { return a.confidence < b.confidence; });
    
    return *bestChord;
}

std::vector<ChordInfo> ChordDetector::detectAllChords(const std::vector<float>& frequencies, const std::vector<float>& magnitudes)
{
    std::vector<ChordInfo> detectedChords;
    auto notes = extractNotes(frequencies, magnitudes);
    
    if (notes.size() < 2)
        return detectedChords;
    
    // Sort notes
    std::sort(notes.begin(), notes.end());
    
    // Try each note as root
    for (int rootNote : notes)
    {
        // Transpose notes to start from root
        std::vector<int> transposedNotes;
        for (int note : notes)
        {
            int transposed = (note - rootNote + 12) % 12;
            if (transposed != 0) // Don't add root twice
                transposedNotes.push_back(transposed);
        }
        transposedNotes.push_back(0); // Add root
        
        // Get intervals
        auto intervals = getNoteIntervals(transposedNotes);
        
        // Check against chord database
        for (const auto& chord : chordDatabase)
        {
            if (intervalsMatch(intervals, chord.second))
            {
                float confidence = calculateChordConfidence(notes, chord.second);
                if (confidence >= confidenceThreshold)
                {
                    detectedChords.emplace_back(chord.first, chord.second, confidence, rootNote);
                }
            }
        }
    }
    
    return detectedChords;
}

void ChordDetector::analyzeHarmonics(const std::vector<float>& frequencies, const std::vector<float>& magnitudes)
{
    harmonicFrequencies.clear();
    harmonicMagnitudes.clear();
    
    // Find peaks in magnitude spectrum
    for (size_t i = 1; i < magnitudes.size() - 1; ++i)
    {
        if (magnitudes[i] > detectionThreshold &&
            magnitudes[i] > magnitudes[i-1] &&
            magnitudes[i] > magnitudes[i+1])
        {
            harmonicFrequencies.push_back(frequencies[i]);
            harmonicMagnitudes.push_back(magnitudes[i]);
        }
    }
}

std::vector<int> ChordDetector::extractNotes(const std::vector<float>& frequencies, const std::vector<float>& magnitudes)
{
    std::vector<int> notes;
    std::vector<bool> noteDetected(12, false);
    
    for (size_t i = 0; i < harmonicFrequencies.size(); ++i)
    {
        float frequency = harmonicFrequencies[i];
        float magnitude = harmonicMagnitudes[i];
        
        if (magnitude < detectionThreshold)
            continue;
        
        float note = frequencyToNote(frequency);
        int midiNote = noteToMidi(note);
        int noteClass = midiNote % 12;
        
        if (!noteDetected[noteClass])
        {
            notes.push_back(noteClass);
            noteDetected[noteClass] = true;
        }
    }
    
    return notes;
}

float ChordDetector::calculateChordConfidence(const std::vector<int>& detectedNotes, const std::vector<int>& chordIntervals)
{
    if (detectedNotes.empty() || chordIntervals.empty())
        return 0.0f;
    
    int matches = 0;
    int totalNotes = chordIntervals.size();
    
    for (int interval : chordIntervals)
    {
        if (std::find(detectedNotes.begin(), detectedNotes.end(), interval) != detectedNotes.end())
        {
            matches++;
        }
    }
    
    float baseConfidence = (float)matches / totalNotes;
    
    // Boost confidence for more notes detected
    float noteBonus = std::min((float)detectedNotes.size() / 6.0f, 1.0f) * 0.2f;
    
    return std::min(baseConfidence + noteBonus, 1.0f);
}

void ChordDetector::addCustomChord(const std::string& name, const std::vector<int>& intervals)
{
    chordDatabase[name] = intervals;
}

void ChordDetector::removeCustomChord(const std::string& name)
{
    chordDatabase.erase(name);
}

std::vector<std::string> ChordDetector::getAvailableChords() const
{
    std::vector<std::string> chords;
    for (const auto& chord : chordDatabase)
    {
        chords.push_back(chord.first);
    }
    return chords;
}

void ChordDetector::setDetectionThreshold(float threshold)
{
    detectionThreshold = threshold;
}

void ChordDetector::setHarmonicWeight(float weight)
{
    harmonicWeight = weight;
}

void ChordDetector::setConfidenceThreshold(float threshold)
{
    confidenceThreshold = threshold;
}

void ChordDetector::enableExtendedChords(bool enabled)
{
    extendedChordsEnabled = enabled;
}

void ChordDetector::initializeChordDatabase()
{
    // Major chords
    chordDatabase["maj"] = {0, 4, 7};
    chordDatabase["maj7"] = {0, 4, 7, 11};
    chordDatabase["maj9"] = {0, 4, 7, 11, 14};
    chordDatabase["maj13"] = {0, 4, 7, 11, 14, 21};
    
    // Minor chords
    chordDatabase["min"] = {0, 3, 7};
    chordDatabase["min7"] = {0, 3, 7, 10};
    chordDatabase["min9"] = {0, 3, 7, 10, 14};
    chordDatabase["min11"] = {0, 3, 7, 10, 14, 17};
    
    // Dominant chords
    chordDatabase["7"] = {0, 4, 7, 10};
    chordDatabase["9"] = {0, 4, 7, 10, 14};
    chordDatabase["11"] = {0, 4, 7, 10, 14, 17};
    chordDatabase["13"] = {0, 4, 7, 10, 14, 17, 21};
    
    // Diminished chords
    chordDatabase["dim"] = {0, 3, 6};
    chordDatabase["dim7"] = {0, 3, 6, 9};
    chordDatabase["hdim7"] = {0, 3, 6, 10};
    
    // Augmented chords
    chordDatabase["aug"] = {0, 4, 8};
    chordDatabase["aug7"] = {0, 4, 8, 10};
    
    // Suspended chords
    chordDatabase["sus2"] = {0, 2, 7};
    chordDatabase["sus4"] = {0, 5, 7};
    chordDatabase["sus2sus4"] = {0, 2, 5, 7};
    
    // Power chords
    chordDatabase["5"] = {0, 7};
    
    // Extended chords (if enabled)
    if (extendedChordsEnabled)
    {
        chordDatabase["maj7#11"] = {0, 4, 7, 11, 18};
        chordDatabase["min7b5"] = {0, 3, 6, 10};
        chordDatabase["7#5"] = {0, 4, 8, 10};
        chordDatabase["7b5"] = {0, 4, 6, 10};
        chordDatabase["7#9"] = {0, 4, 7, 10, 15};
        chordDatabase["7b9"] = {0, 4, 7, 10, 13};
        chordDatabase["7#11"] = {0, 4, 7, 10, 18};
        chordDatabase["7b13"] = {0, 4, 7, 10, 20};
    }
}

float ChordDetector::frequencyToNote(float frequency)
{
    if (frequency <= 0.0f)
        return 0.0f;
    
    return 12.0f * std::log2(frequency / 440.0f) + 69.0f;
}

int ChordDetector::noteToMidi(float note)
{
    return static_cast<int>(std::round(note));
}

float ChordDetector::midiToFrequency(int midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

std::vector<int> ChordDetector::getNoteIntervals(const std::vector<int>& notes)
{
    if (notes.size() < 2)
        return {};
    
    std::vector<int> intervals;
    for (size_t i = 1; i < notes.size(); ++i)
    {
        intervals.push_back((notes[i] - notes[0] + 12) % 12);
    }
    
    return intervals;
}

bool ChordDetector::intervalsMatch(const std::vector<int>& intervals1, const std::vector<int>& intervals2, float tolerance)
{
    if (intervals1.size() != intervals2.size())
        return false;
    
    for (size_t i = 0; i < intervals1.size(); ++i)
    {
        if (std::abs(intervals1[i] - intervals2[i]) > tolerance)
            return false;
    }
    
    return true;
}

float ChordDetector::calculateNoteStrength(float frequency, float magnitude)
{
    // Simple strength calculation based on magnitude
    return magnitude;
} 