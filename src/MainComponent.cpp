#include "MainComponent.h"

MainComponent::MainComponent()
{
    setupAudio();
    setupUI();
    setSize(1200, 800);
}

MainComponent::~MainComponent()
{
    stopAudio();
    shutdownAudio();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1e1e1e));
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("ToneTrigger", getLocalBounds().removeFromTop(50), 
               juce::Justification::centred, true);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Title area
    auto titleArea = bounds.removeFromTop(50);
    statusLabel.setBounds(titleArea.reduced(10));
    
    // Control area
    auto controlArea = bounds.removeFromTop(80);
    startStopButton.setBounds(controlArea.removeFromLeft(120).reduced(10));
    
    auto gainArea = controlArea.removeFromRight(300);
    inputGainSlider.setBounds(gainArea.removeFromTop(35).reduced(10));
    outputGainSlider.setBounds(gainArea.reduced(10));
    
    // Main tabbed area
    tabbedComponent.setBounds(bounds.reduced(10));
}

void MainComponent::setupAudio()
{
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    audioSourcePlayer = std::make_unique<juce::AudioSourcePlayer>();
    audioProcessor = std::make_unique<AudioProcessor>();
    
    deviceManager->addAudioCallback(audioSourcePlayer.get());
    deviceManager->addChangeListener(this);
    
    audioSourcePlayer->setSource(audioProcessor.get());
    
    // Initialize with default audio device
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    setup.sampleRate = 44100.0;
    setup.bufferSize = 256;
    deviceManager->setAudioDeviceSetup(setup, true);
}

void MainComponent::setupUI()
{
    // Create UI components
    triggerPanel = std::make_unique<TriggerPanel>(audioProcessor.get());
    effectPanel = std::make_unique<EffectPanel>(audioProcessor.get());
    audioSettingsPanel = std::make_unique<AudioSettingsPanel>(deviceManager.get());
    
    // Setup tabbed component
    tabbedComponent.addTab("Triggers", juce::Colour(0xff2d2d2d), triggerPanel.get(), false);
    tabbedComponent.addTab("Effects", juce::Colour(0xff2d2d2d), effectPanel.get(), false);
    tabbedComponent.addTab("Audio Settings", juce::Colour(0xff2d2d2d), audioSettingsPanel.get(), false);
    addAndMakeVisible(tabbedComponent);
    
    // Setup start/stop button
    startStopButton.setButtonText("Start");
    startStopButton.onClick = [this]() {
        if (isPlaying)
            stopAudio();
        else
            startAudio();
    };
    addAndMakeVisible(startStopButton);
    
    // Setup status label
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    addAndMakeVisible(statusLabel);
    
    // Setup gain sliders
    inputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    inputGainSlider.setRange(0.0, 2.0, 0.01);
    inputGainSlider.setValue(1.0);
    inputGainSlider.onValueChange = [this]() {
        if (audioProcessor)
            audioProcessor->setInputGain(static_cast<float>(inputGainSlider.getValue()));
    };
    addAndMakeVisible(inputGainSlider);
    
    outputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    outputGainSlider.setRange(0.0, 2.0, 0.01);
    outputGainSlider.setValue(1.0);
    outputGainSlider.onValueChange = [this]() {
        if (audioProcessor)
            audioProcessor->setOutputGain(static_cast<float>(outputGainSlider.getValue()));
    };
    addAndMakeVisible(outputGainSlider);
}

void MainComponent::startAudio()
{
    if (!isPlaying)
    {
        deviceManager->startDevice();
        isPlaying = true;
        startStopButton.setButtonText("Stop");
        updateStatus();
    }
}

void MainComponent::stopAudio()
{
    if (isPlaying)
    {
        deviceManager->stopDevice();
        isPlaying = false;
        startStopButton.setButtonText("Start");
        updateStatus();
    }
}

void MainComponent::updateStatus()
{
    if (isPlaying)
    {
        statusLabel.setText("Playing", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    }
    else
    {
        statusLabel.setText("Stopped", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    }
}

void MainComponent::audioDeviceListChanged()
{
    // Handle audio device list changes
}

void MainComponent::audioDeviceSetupChanged()
{
    // Handle audio device setup changes
    updateStatus();
} 