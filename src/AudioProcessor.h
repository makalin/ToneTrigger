#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <memory>
#include <vector>

class TriggerManager;
class EffectProcessor;
class AudioAnalyzer;

class AudioProcessor : public juce::AudioSource
{
public:
    AudioProcessor();
    ~AudioProcessor() override;

    // AudioSource methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Audio processing
    void setInputGain(float gain);
    void setOutputGain(float gain);
    float getInputGain() const { return inputGain; }
    float getOutputGain() const { return outputGain; }

    // Trigger management
    void addNoteTrigger(int note, int effectId);
    void addChordTrigger(const std::vector<int>& notes, int effectId);
    void addMelodyTrigger(const std::vector<int>& sequence, int effectId);
    void removeTrigger(int triggerId);

    // Effect management
    void setEffectEnabled(int effectId, bool enabled);
    void setEffectParameter(int effectId, int parameterId, float value);
    bool isEffectEnabled(int effectId) const;

    // Analysis
    float getCurrentNote() const;
    float getCurrentChord() const;
    float getCurrentMelody() const;

    // Access to components
    TriggerManager* getTriggerManager() const { return triggerManager.get(); }
    EffectProcessor* getEffectProcessor() const { return effectProcessor.get(); }
    AudioAnalyzer* getAudioAnalyzer() const { return audioAnalyzer.get(); }

private:
    // Audio parameters
    float inputGain = 1.0f;
    float outputGain = 1.0f;
    double sampleRate = 44100.0;
    int blockSize = 256;

    // Processing components
    std::unique_ptr<TriggerManager> triggerManager;
    std::unique_ptr<EffectProcessor> effectProcessor;
    std::unique_ptr<AudioAnalyzer> audioAnalyzer;

    // Audio buffers
    juce::AudioBuffer<float> inputBuffer;
    juce::AudioBuffer<float> outputBuffer;
    juce::AudioBuffer<float> tempBuffer;

    // Processing methods
    void processAudio(const juce::AudioSourceChannelInfo& bufferToFill);
    void applyInputGain(juce::AudioBuffer<float>& buffer);
    void applyOutputGain(juce::AudioBuffer<float>& buffer);
    void checkTriggers();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
}; 