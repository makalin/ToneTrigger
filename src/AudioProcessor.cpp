#include "AudioProcessor.h"
#include "TriggerManager.h"
#include "EffectProcessor.h"
#include "AudioAnalyzer.h"

AudioProcessor::AudioProcessor()
{
    triggerManager = std::make_unique<TriggerManager>();
    effectProcessor = std::make_unique<EffectProcessor>();
    audioAnalyzer = std::make_unique<AudioAnalyzer>();
}

AudioProcessor::~AudioProcessor()
{
}

void AudioProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlockExpected;

    // Prepare buffers
    inputBuffer.setSize(2, samplesPerBlockExpected);
    outputBuffer.setSize(2, samplesPerBlockExpected);
    tempBuffer.setSize(2, samplesPerBlockExpected);

    // Prepare components
    if (triggerManager)
        triggerManager->prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (effectProcessor)
        effectProcessor->prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (audioAnalyzer)
        audioAnalyzer->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioProcessor::releaseResources()
{
    inputBuffer.setSize(0, 0);
    outputBuffer.setSize(0, 0);
    tempBuffer.setSize(0, 0);

    if (triggerManager)
        triggerManager->releaseResources();
    if (effectProcessor)
        effectProcessor->releaseResources();
    if (audioAnalyzer)
        audioAnalyzer->releaseResources();
}

void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    processAudio(bufferToFill);
}

void AudioProcessor::setInputGain(float gain)
{
    inputGain = juce::jlimit(0.0f, 10.0f, gain);
}

void AudioProcessor::setOutputGain(float gain)
{
    outputGain = juce::jlimit(0.0f, 10.0f, gain);
}

void AudioProcessor::addNoteTrigger(int note, int effectId)
{
    if (triggerManager)
        triggerManager->addNoteTrigger(note, effectId);
}

void AudioProcessor::addChordTrigger(const std::vector<int>& notes, int effectId)
{
    if (triggerManager)
        triggerManager->addChordTrigger(notes, effectId);
}

void AudioProcessor::addMelodyTrigger(const std::vector<int>& sequence, int effectId)
{
    if (triggerManager)
        triggerManager->addMelodyTrigger(sequence, effectId);
}

void AudioProcessor::removeTrigger(int triggerId)
{
    if (triggerManager)
        triggerManager->removeTrigger(triggerId);
}

void AudioProcessor::setEffectEnabled(int effectId, bool enabled)
{
    if (effectProcessor)
        effectProcessor->setEffectEnabled(effectId, enabled);
}

void AudioProcessor::setEffectParameter(int effectId, int parameterId, float value)
{
    if (effectProcessor)
        effectProcessor->setParameter(effectId, parameterId, value);
}

bool AudioProcessor::isEffectEnabled(int effectId) const
{
    if (effectProcessor)
        return effectProcessor->isEffectEnabled(effectId);
    return false;
}

float AudioProcessor::getCurrentNote() const
{
    if (audioAnalyzer)
        return audioAnalyzer->getCurrentNote();
    return -1.0f;
}

float AudioProcessor::getCurrentChord() const
{
    if (audioAnalyzer)
        return audioAnalyzer->getCurrentChord();
    return -1.0f;
}

float AudioProcessor::getCurrentMelody() const
{
    if (audioAnalyzer)
        return audioAnalyzer->getCurrentMelody();
    return -1.0f;
}

void AudioProcessor::processAudio(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear output buffer
    outputBuffer.clear();

    // Copy input to our working buffer
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        const float* inputChannel = bufferToFill.buffer->getReadPointer(channel);
        float* outputChannel = outputBuffer.getWritePointer(channel);
        
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            outputChannel[sample] = inputChannel[sample];
        }
    }

    // Apply input gain
    applyInputGain(outputBuffer);

    // Analyze audio for triggers
    if (audioAnalyzer)
    {
        audioAnalyzer->processAudio(outputBuffer);
        checkTriggers();
    }

    // Apply effects
    if (effectProcessor)
    {
        effectProcessor->processAudio(outputBuffer);
    }

    // Apply output gain
    applyOutputGain(outputBuffer);

    // Copy processed audio back to output
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        const float* processedChannel = outputBuffer.getReadPointer(channel);
        float* outputChannel = bufferToFill.buffer->getWritePointer(channel);
        
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            outputChannel[sample] = processedChannel[sample];
        }
    }
}

void AudioProcessor::applyInputGain(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= inputGain;
        }
    }
}

void AudioProcessor::applyOutputGain(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= outputGain;
        }
    }
}

void AudioProcessor::checkTriggers()
{
    if (!triggerManager || !audioAnalyzer)
        return;

    // Get current detected note/chord/melody
    float currentNote = audioAnalyzer->getCurrentNote();
    float currentChord = audioAnalyzer->getCurrentChord();
    float currentMelody = audioAnalyzer->getCurrentMelody();

    // Check for triggers
    triggerManager->checkTriggers(currentNote, currentChord, currentMelody);
} 