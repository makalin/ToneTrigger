#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

class AudioVisualizer : public juce::Component, public juce::Timer
{
public:
    enum class VisualizerType
    {
        Waveform,
        Spectrum,
        Waterfall,
        Oscilloscope
    };

    AudioVisualizer();
    ~AudioVisualizer() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Audio data input
    void pushAudioData(const juce::AudioBuffer<float>& buffer);
    void setSampleRate(double sampleRate);

    // Visualizer settings
    void setVisualizerType(VisualizerType type);
    void setUpdateRate(int fps);
    void setSpectrumResolution(int resolution);
    void setWaveformScale(float scale);
    void setColorScheme(const juce::Colour& primary, const juce::Colour& secondary);

    // Display options
    void setShowGrid(bool show);
    void setShowLabels(bool show);
    void setAutoScale(bool autoScale);
    void setPeakHold(bool enabled);

private:
    // Visualizer type
    VisualizerType visualizerType = VisualizerType::Waveform;

    // Audio data
    std::vector<float> audioBuffer;
    std::vector<float> spectrumData;
    std::vector<std::vector<float>> waterfallData;
    double sampleRate = 44100.0;
    int bufferSize = 1024;

    // Display settings
    int updateRate = 30;
    int spectrumResolution = 256;
    float waveformScale = 1.0f;
    juce::Colour primaryColor = juce::Colours::lightblue;
    juce::Colour secondaryColor = juce::Colours::darkblue;
    bool showGrid = true;
    bool showLabels = true;
    bool autoScale = true;
    bool peakHold = false;

    // Peak tracking
    std::vector<float> peakValues;
    std::vector<float> holdValues;

    // Drawing methods
    void drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawSpectrum(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawWaterfall(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawOscilloscope(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawGrid(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawLabels(juce::Graphics& g, const juce::Rectangle<int>& bounds);

    // Analysis methods
    void calculateSpectrum();
    void updateWaterfall();
    float getPeakValue() const;
    void updatePeakHold();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioVisualizer)
}; 