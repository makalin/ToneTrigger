#include "AudioVisualizer.h"
#include <cmath>
#include <algorithm>

AudioVisualizer::AudioVisualizer()
{
    // Initialize buffers
    audioBuffer.resize(bufferSize, 0.0f);
    spectrumData.resize(spectrumResolution, 0.0f);
    peakValues.resize(spectrumResolution, 0.0f);
    holdValues.resize(spectrumResolution, 0.0f);
    
    // Initialize waterfall data
    waterfallData.resize(100); // 100 lines of waterfall
    for (auto& line : waterfallData)
    {
        line.resize(spectrumResolution, 0.0f);
    }
    
    // Start timer for updates
    startTimerHz(updateRate);
}

AudioVisualizer::~AudioVisualizer()
{
    stopTimer();
}

void AudioVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Fill background
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRect(bounds);
    
    // Draw grid if enabled
    if (showGrid)
        drawGrid(g, bounds.toType<int>());
    
    // Draw visualizer based on type
    switch (visualizerType)
    {
        case VisualizerType::Waveform:
            drawWaveform(g, bounds.toType<int>());
            break;
        case VisualizerType::Spectrum:
            drawSpectrum(g, bounds.toType<int>());
            break;
        case VisualizerType::Waterfall:
            drawWaterfall(g, bounds.toType<int>());
            break;
        case VisualizerType::Oscilloscope:
            drawOscilloscope(g, bounds.toType<int>());
            break;
    }
    
    // Draw labels if enabled
    if (showLabels)
        drawLabels(g, bounds.toType<int>());
}

void AudioVisualizer::resized()
{
    // Component resized - update if needed
}

void AudioVisualizer::timerCallback()
{
    repaint();
}

void AudioVisualizer::pushAudioData(const juce::AudioBuffer<float>& buffer)
{
    // Convert stereo to mono and add to buffer
    int numSamples = std::min(buffer.getNumSamples(), bufferSize);
    
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            sample += buffer.getSample(channel, i);
        }
        sample /= buffer.getNumChannels();
        
        // Shift buffer and add new sample
        audioBuffer.erase(audioBuffer.begin());
        audioBuffer.push_back(sample);
    }
    
    // Update spectrum if needed
    if (visualizerType == VisualizerType::Spectrum || visualizerType == VisualizerType::Waterfall)
    {
        calculateSpectrum();
    }
    
    // Update waterfall if needed
    if (visualizerType == VisualizerType::Waterfall)
    {
        updateWaterfall();
    }
}

void AudioVisualizer::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void AudioVisualizer::setVisualizerType(VisualizerType type)
{
    visualizerType = type;
    repaint();
}

void AudioVisualizer::setUpdateRate(int fps)
{
    updateRate = fps;
    stopTimer();
    startTimerHz(updateRate);
}

void AudioVisualizer::setSpectrumResolution(int resolution)
{
    spectrumResolution = resolution;
    spectrumData.resize(resolution, 0.0f);
    peakValues.resize(resolution, 0.0f);
    holdValues.resize(resolution, 0.0f);
    
    for (auto& line : waterfallData)
    {
        line.resize(resolution, 0.0f);
    }
}

void AudioVisualizer::setWaveformScale(float scale)
{
    waveformScale = scale;
}

void AudioVisualizer::setColorScheme(const juce::Colour& primary, const juce::Colour& secondary)
{
    primaryColor = primary;
    secondaryColor = secondary;
}

void AudioVisualizer::setShowGrid(bool show)
{
    showGrid = show;
}

void AudioVisualizer::setShowLabels(bool show)
{
    showLabels = show;
}

void AudioVisualizer::setAutoScale(bool autoScale)
{
    this->autoScale = autoScale;
}

void AudioVisualizer::setPeakHold(bool enabled)
{
    peakHold = enabled;
}

void AudioVisualizer::drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(primaryColor);
    
    juce::Path waveformPath;
    bool pathStarted = false;
    
    float scale = autoScale ? waveformScale / getPeakValue() : waveformScale;
    if (scale > 10.0f) scale = 10.0f; // Limit maximum scale
    
    for (int i = 0; i < audioBuffer.size(); ++i)
    {
        float x = bounds.getX() + (float)i / audioBuffer.size() * bounds.getWidth();
        float y = bounds.getCentreY() - audioBuffer[i] * scale * bounds.getHeight() * 0.5f;
        
        if (!pathStarted)
        {
            waveformPath.startNewSubPath(x, y);
            pathStarted = true;
        }
        else
        {
            waveformPath.lineTo(x, y);
        }
    }
    
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
}

void AudioVisualizer::drawSpectrum(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    float barWidth = (float)bounds.getWidth() / spectrumData.size();
    
    for (int i = 0; i < spectrumData.size(); ++i)
    {
        float x = bounds.getX() + i * barWidth;
        float height = spectrumData[i] * bounds.getHeight();
        
        // Draw main bar
        g.setColour(primaryColor);
        g.fillRect(x, bounds.getBottom() - height, barWidth - 1, height);
        
        // Draw peak hold if enabled
        if (peakHold && holdValues[i] > 0.0f)
        {
            g.setColour(secondaryColor);
            float peakHeight = holdValues[i] * bounds.getHeight();
            g.fillRect(x, bounds.getBottom() - peakHeight, barWidth - 1, 2.0f);
        }
    }
}

void AudioVisualizer::drawWaterfall(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    float lineHeight = (float)bounds.getHeight() / waterfallData.size();
    
    for (int line = 0; line < waterfallData.size(); ++line)
    {
        float y = bounds.getY() + line * lineHeight;
        
        for (int i = 0; i < waterfallData[line].size(); ++i)
        {
            float x = bounds.getX() + (float)i / waterfallData[line].size() * bounds.getWidth();
            float intensity = waterfallData[line][i];
            
            if (intensity > 0.0f)
            {
                juce::Colour color = primaryColor.withAlpha(intensity);
                g.setColour(color);
                g.fillRect(x, y, 1.0f, lineHeight);
            }
        }
    }
}

void AudioVisualizer::drawOscilloscope(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(primaryColor);
    
    juce::Path scopePath;
    bool pathStarted = false;
    
    for (int i = 0; i < audioBuffer.size(); i += 2) // Skip samples for performance
    {
        float x = bounds.getX() + (float)i / audioBuffer.size() * bounds.getWidth();
        float y = bounds.getCentreY() - audioBuffer[i] * bounds.getHeight() * 0.5f;
        
        if (!pathStarted)
        {
            scopePath.startNewSubPath(x, y);
            pathStarted = true;
        }
        else
        {
            scopePath.lineTo(x, y);
        }
    }
    
    g.strokePath(scopePath, juce::PathStrokeType(1.0f));
}

void AudioVisualizer::drawGrid(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(juce::Colours::darkgrey);
    
    // Vertical lines
    for (int i = 1; i < 10; ++i)
    {
        float x = bounds.getX() + (float)i / 10.0f * bounds.getWidth();
        g.drawVerticalLine((int)x, bounds.getY(), bounds.getBottom());
    }
    
    // Horizontal lines
    for (int i = 1; i < 8; ++i)
    {
        float y = bounds.getY() + (float)i / 8.0f * bounds.getHeight();
        g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
    }
}

void AudioVisualizer::drawLabels(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    
    // Draw frequency labels for spectrum
    if (visualizerType == VisualizerType::Spectrum || visualizerType == VisualizerType::Waterfall)
    {
        for (int i = 0; i <= 10; ++i)
        {
            float freq = i * sampleRate / 20.0f; // 0 to Nyquist
            float x = bounds.getX() + (float)i / 10.0f * bounds.getWidth();
            
            juce::String label;
            if (freq >= 1000.0f)
                label = juce::String(freq / 1000.0f, 1) + "k";
            else
                label = juce::String((int)freq);
            
            g.drawText(label, (int)x - 20, bounds.getBottom() + 5, 40, 15, juce::Justification::centred);
        }
    }
}

void AudioVisualizer::calculateSpectrum()
{
    // Simple FFT-like calculation (simplified for performance)
    for (int i = 0; i < spectrumData.size(); ++i)
    {
        float sum = 0.0f;
        float freq = (float)i / spectrumData.size() * sampleRate * 0.5f;
        
        for (int j = 0; j < audioBuffer.size(); ++j)
        {
            float phase = 2.0f * M_PI * freq * j / sampleRate;
            sum += audioBuffer[j] * std::cos(phase);
        }
        
        spectrumData[i] = std::abs(sum) / audioBuffer.size();
        
        // Update peak hold
        if (spectrumData[i] > peakValues[i])
            peakValues[i] = spectrumData[i];
        
        if (peakHold)
        {
            if (spectrumData[i] > holdValues[i])
                holdValues[i] = spectrumData[i];
            else
                holdValues[i] *= 0.995f; // Decay
        }
    }
}

void AudioVisualizer::updateWaterfall()
{
    // Shift waterfall data down
    for (int i = waterfallData.size() - 1; i > 0; --i)
    {
        waterfallData[i] = waterfallData[i - 1];
    }
    
    // Add new spectrum line at top
    waterfallData[0] = spectrumData;
}

float AudioVisualizer::getPeakValue() const
{
    if (audioBuffer.empty())
        return 1.0f;
    
    return *std::max_element(audioBuffer.begin(), audioBuffer.end());
}

void AudioVisualizer::updatePeakHold()
{
    if (!peakHold)
    {
        std::fill(holdValues.begin(), holdValues.end(), 0.0f);
    }
} 