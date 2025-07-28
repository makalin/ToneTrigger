#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <map>
#include <functional>

enum class TriggerType
{
    Note,
    Chord,
    Melody
};

struct Trigger
{
    int id;
    TriggerType type;
    std::vector<int> notes;
    int effectId;
    bool enabled;
    float threshold;
    int duration;
    
    Trigger(int triggerId, TriggerType triggerType, const std::vector<int>& triggerNotes, 
            int triggerEffectId, float triggerThreshold = 0.5f, int triggerDuration = 100)
        : id(triggerId), type(triggerType), notes(triggerNotes), effectId(triggerEffectId),
          enabled(true), threshold(triggerThreshold), duration(triggerDuration) {}
};

class TriggerManager
{
public:
    TriggerManager();
    ~TriggerManager();

    // Setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources();

    // Trigger management
    int addNoteTrigger(int note, int effectId, float threshold = 0.5f);
    int addChordTrigger(const std::vector<int>& notes, int effectId, float threshold = 0.5f);
    int addMelodyTrigger(const std::vector<int>& sequence, int effectId, float threshold = 0.5f);
    void removeTrigger(int triggerId);
    void enableTrigger(int triggerId, bool enabled);
    void setTriggerThreshold(int triggerId, float threshold);

    // Trigger checking
    void checkTriggers(float currentNote, float currentChord, float currentMelody);
    
    // Callbacks
    void setTriggerCallback(std::function<void(int effectId, bool activated)> callback);
    
    // Getters
    const std::vector<Trigger>& getTriggers() const { return triggers; }
    bool isTriggerActive(int triggerId) const;
    int getActiveEffectId() const { return activeEffectId; }

private:
    // Triggers
    std::vector<Trigger> triggers;
    int nextTriggerId = 1;
    
    // State
    int activeEffectId = -1;
    std::map<int, int> triggerTimers; // triggerId -> timer
    std::map<int, bool> triggerStates; // triggerId -> active state
    
    // Audio parameters
    double sampleRate = 44100.0;
    int blockSize = 256;
    
    // Callback
    std::function<void(int effectId, bool activated)> triggerCallback;
    
    // Helper methods
    bool checkNoteTrigger(const Trigger& trigger, float currentNote);
    bool checkChordTrigger(const Trigger& trigger, float currentChord);
    bool checkMelodyTrigger(const Trigger& trigger, float currentMelody);
    void activateTrigger(const Trigger& trigger);
    void deactivateTrigger(const Trigger& trigger);
    void updateTimers();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerManager)
}; 