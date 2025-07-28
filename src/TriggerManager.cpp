#include "TriggerManager.h"

TriggerManager::TriggerManager()
{
}

TriggerManager::~TriggerManager()
{
}

void TriggerManager::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;
}

void TriggerManager::releaseResources()
{
    triggers.clear();
    triggerTimers.clear();
    triggerStates.clear();
    activeEffectId = -1;
}

int TriggerManager::addNoteTrigger(int note, int effectId, float threshold)
{
    std::vector<int> notes = {note};
    Trigger trigger(nextTriggerId++, TriggerType::Note, notes, effectId, threshold);
    triggers.push_back(trigger);
    return trigger.id;
}

int TriggerManager::addChordTrigger(const std::vector<int>& notes, int effectId, float threshold)
{
    Trigger trigger(nextTriggerId++, TriggerType::Chord, notes, effectId, threshold);
    triggers.push_back(trigger);
    return trigger.id;
}

int TriggerManager::addMelodyTrigger(const std::vector<int>& sequence, int effectId, float threshold)
{
    Trigger trigger(nextTriggerId++, TriggerType::Melody, sequence, effectId, threshold);
    triggers.push_back(trigger);
    return trigger.id;
}

void TriggerManager::removeTrigger(int triggerId)
{
    auto it = std::find_if(triggers.begin(), triggers.end(),
                          [triggerId](const Trigger& t) { return t.id == triggerId; });
    if (it != triggers.end())
    {
        deactivateTrigger(*it);
        triggers.erase(it);
    }
}

void TriggerManager::enableTrigger(int triggerId, bool enabled)
{
    auto it = std::find_if(triggers.begin(), triggers.end(),
                          [triggerId](const Trigger& t) { return t.id == triggerId; });
    if (it != triggers.end())
    {
        it->enabled = enabled;
        if (!enabled)
            deactivateTrigger(*it);
    }
}

void TriggerManager::setTriggerThreshold(int triggerId, float threshold)
{
    auto it = std::find_if(triggers.begin(), triggers.end(),
                          [triggerId](const Trigger& t) { return t.id == triggerId; });
    if (it != triggers.end())
    {
        it->threshold = juce::jlimit(0.0f, 1.0f, threshold);
    }
}

void TriggerManager::checkTriggers(float currentNote, float currentChord, float currentMelody)
{
    updateTimers();
    
    for (const auto& trigger : triggers)
    {
        if (!trigger.enabled)
            continue;
            
        bool shouldActivate = false;
        
        switch (trigger.type)
        {
            case TriggerType::Note:
                shouldActivate = checkNoteTrigger(trigger, currentNote);
                break;
            case TriggerType::Chord:
                shouldActivate = checkChordTrigger(trigger, currentChord);
                break;
            case TriggerType::Melody:
                shouldActivate = checkMelodyTrigger(trigger, currentMelody);
                break;
        }
        
        if (shouldActivate)
        {
            activateTrigger(trigger);
        }
        else
        {
            deactivateTrigger(trigger);
        }
    }
}

void TriggerManager::setTriggerCallback(std::function<void(int effectId, bool activated)> callback)
{
    triggerCallback = callback;
}

bool TriggerManager::isTriggerActive(int triggerId) const
{
    auto it = triggerStates.find(triggerId);
    return it != triggerStates.end() && it->second;
}

bool TriggerManager::checkNoteTrigger(const Trigger& trigger, float currentNote)
{
    if (currentNote < 0)
        return false;
        
    // Check if current note matches any note in the trigger
    for (int note : trigger.notes)
    {
        if (std::abs(currentNote - note) < 0.5f) // Allow some tolerance
            return true;
    }
    return false;
}

bool TriggerManager::checkChordTrigger(const Trigger& trigger, float currentChord)
{
    if (currentChord < 0)
        return false;
        
    // For now, we'll use a simple approach where we check if the chord root matches
    // In a real implementation, you'd want more sophisticated chord detection
    int chordRoot = static_cast<int>(currentChord) % 12;
    
    for (int note : trigger.notes)
    {
        if (note % 12 == chordRoot)
            return true;
    }
    return false;
}

bool TriggerManager::checkMelodyTrigger(const Trigger& trigger, float currentMelody)
{
    if (currentMelody < 0)
        return false;
        
    // For melody triggers, we'd need to maintain a history of recent notes
    // and check if they match the sequence. This is a simplified version.
    // In a real implementation, you'd want to track the melody sequence over time.
    
    // For now, just check if the current melody note matches the first note in the sequence
    if (!trigger.notes.empty())
    {
        return std::abs(currentMelody - trigger.notes[0]) < 0.5f;
    }
    return false;
}

void TriggerManager::activateTrigger(const Trigger& trigger)
{
    if (triggerStates[trigger.id])
        return; // Already active
        
    triggerStates[trigger.id] = true;
    triggerTimers[trigger.id] = trigger.duration;
    
    // Set as active effect
    activeEffectId = trigger.effectId;
    
    // Call callback
    if (triggerCallback)
        triggerCallback(trigger.effectId, true);
}

void TriggerManager::deactivateTrigger(const Trigger& trigger)
{
    if (!triggerStates[trigger.id])
        return; // Already inactive
        
    triggerStates[trigger.id] = false;
    triggerTimers.erase(trigger.id);
    
    // If this was the active effect, clear it
    if (activeEffectId == trigger.effectId)
        activeEffectId = -1;
    
    // Call callback
    if (triggerCallback)
        triggerCallback(trigger.effectId, false);
}

void TriggerManager::updateTimers()
{
    std::vector<int> expiredTriggers;
    
    for (auto& timer : triggerTimers)
    {
        timer.second -= blockSize;
        if (timer.second <= 0)
        {
            expiredTriggers.push_back(timer.first);
        }
    }
    
    // Deactivate expired triggers
    for (int triggerId : expiredTriggers)
    {
        auto it = std::find_if(triggers.begin(), triggers.end(),
                              [triggerId](const Trigger& t) { return t.id == triggerId; });
        if (it != triggers.end())
        {
            deactivateTrigger(*it);
        }
    }
} 