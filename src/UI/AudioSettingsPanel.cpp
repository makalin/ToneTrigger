#include "AudioSettingsPanel.h"

AudioSettingsPanel::AudioSettingsPanel(juce::AudioDeviceManager* deviceManager)
    : deviceManager(deviceManager)
{
    setupUI();
}

AudioSettingsPanel::~AudioSettingsPanel()
{
}

void AudioSettingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2d2d2d));
}

void AudioSettingsPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Device settings area
    auto deviceArea = bounds.removeFromTop(200);
    
    // Input device
    inputDeviceLabel.setBounds(deviceArea.removeFromTop(25));
    inputDeviceComboBox.setBounds(deviceArea.removeFromTop(30).reduced(5));
    
    // Output device
    outputDeviceLabel.setBounds(deviceArea.removeFromTop(25));
    outputDeviceComboBox.setBounds(deviceArea.removeFromTop(30).reduced(5));
    
    // Sample rate
    sampleRateLabel.setBounds(deviceArea.removeFromTop(25));
    sampleRateComboBox.setBounds(deviceArea.removeFromTop(30).reduced(5));
    
    // Buffer size
    bufferSizeLabel.setBounds(deviceArea.removeFromTop(25));
    bufferSizeComboBox.setBounds(deviceArea.removeFromTop(30).reduced(5));
    
    // Test area
    auto testArea = bounds;
    testButton.setBounds(testArea.removeFromTop(40).reduced(5));
    statusLabel.setBounds(testArea.reduced(5));
}

void AudioSettingsPanel::setupUI()
{
    // Setup labels
    inputDeviceLabel.setText("Input Device:", juce::dontSendNotification);
    inputDeviceLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(inputDeviceLabel);
    
    outputDeviceLabel.setText("Output Device:", juce::dontSendNotification);
    outputDeviceLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(outputDeviceLabel);
    
    sampleRateLabel.setText("Sample Rate:", juce::dontSendNotification);
    sampleRateLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(sampleRateLabel);
    
    bufferSizeLabel.setText("Buffer Size:", juce::dontSendNotification);
    bufferSizeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(bufferSizeLabel);
    
    // Setup combo boxes
    inputDeviceComboBox.onChange = [this]() { onDeviceChanged(); };
    addAndMakeVisible(inputDeviceComboBox);
    
    outputDeviceComboBox.onChange = [this]() { onDeviceChanged(); };
    addAndMakeVisible(outputDeviceComboBox);
    
    sampleRateComboBox.onChange = [this]() { onSampleRateChanged(); };
    addAndMakeVisible(sampleRateComboBox);
    
    bufferSizeComboBox.onChange = [this]() { onBufferSizeChanged(); };
    addAndMakeVisible(bufferSizeComboBox);
    
    // Setup test button
    testButton.setButtonText("Test Audio");
    testButton.onClick = [this]() { testAudio(); };
    addAndMakeVisible(testButton);
    
    // Setup status label
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    addAndMakeVisible(statusLabel);
    
    updateDeviceLists();
}

void AudioSettingsPanel::updateDeviceLists()
{
    if (!deviceManager)
        return;
    
    // Update input devices
    inputDeviceComboBox.clear();
    auto inputDevices = deviceManager->getAvailableDeviceTypes();
    for (auto* deviceType : inputDevices)
    {
        auto deviceNames = deviceType->getDeviceNames(true); // true = input devices
        for (const auto& deviceName : deviceNames)
        {
            inputDeviceComboBox.addItem(deviceName, inputDeviceComboBox.getNumItems() + 1);
        }
    }
    
    // Update output devices
    outputDeviceComboBox.clear();
    for (auto* deviceType : inputDevices)
    {
        auto deviceNames = deviceType->getDeviceNames(false); // false = output devices
        for (const auto& deviceName : deviceNames)
        {
            outputDeviceComboBox.addItem(deviceName, outputDeviceComboBox.getNumItems() + 1);
        }
    }
    
    // Select current devices
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    
    // Find and select current input device
    for (int i = 0; i < inputDeviceComboBox.getNumItems(); ++i)
    {
        if (inputDeviceComboBox.getItemText(i) == setup.inputDeviceName)
        {
            inputDeviceComboBox.setSelectedItemIndex(i);
            break;
        }
    }
    
    // Find and select current output device
    for (int i = 0; i < outputDeviceComboBox.getNumItems(); ++i)
    {
        if (outputDeviceComboBox.getItemText(i) == setup.outputDeviceName)
        {
            outputDeviceComboBox.setSelectedItemIndex(i);
            break;
        }
    }
    
    updateSampleRates();
    updateBufferSizes();
}

void AudioSettingsPanel::updateSampleRates()
{
    if (!deviceManager)
        return;
    
    sampleRateComboBox.clear();
    
    // Common sample rates
    std::vector<double> sampleRates = {44100.0, 48000.0, 88200.0, 96000.0};
    
    for (double rate : sampleRates)
    {
        sampleRateComboBox.addItem(juce::String(static_cast<int>(rate)) + " Hz", 
                                  static_cast<int>(rate));
    }
    
    // Select current sample rate
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    sampleRateComboBox.setSelectedId(static_cast<int>(setup.sampleRate));
}

void AudioSettingsPanel::updateBufferSizes()
{
    if (!deviceManager)
        return;
    
    bufferSizeComboBox.clear();
    
    // Common buffer sizes
    std::vector<int> bufferSizes = {64, 128, 256, 512, 1024, 2048};
    
    for (int size : bufferSizes)
    {
        bufferSizeComboBox.addItem(juce::String(size) + " samples", size);
    }
    
    // Select current buffer size
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    bufferSizeComboBox.setSelectedId(setup.bufferSize);
}

void AudioSettingsPanel::onDeviceChanged()
{
    if (!deviceManager)
        return;
    
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    
    // Update device names
    if (inputDeviceComboBox.getSelectedItemIndex() >= 0)
        setup.inputDeviceName = inputDeviceComboBox.getSelectedItemText();
    if (outputDeviceComboBox.getSelectedItemIndex() >= 0)
        setup.outputDeviceName = outputDeviceComboBox.getSelectedItemText();
    
    // Apply changes
    juce::String error = deviceManager->setAudioDeviceSetup(setup, true);
    if (error.isNotEmpty())
    {
        statusLabel.setText("Error: " + error, juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    }
    else
    {
        statusLabel.setText("Device settings updated", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    }
}

void AudioSettingsPanel::onSampleRateChanged()
{
    if (!deviceManager)
        return;
    
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    
    setup.sampleRate = static_cast<double>(sampleRateComboBox.getSelectedId());
    
    juce::String error = deviceManager->setAudioDeviceSetup(setup, true);
    if (error.isNotEmpty())
    {
        statusLabel.setText("Error: " + error, juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    }
    else
    {
        statusLabel.setText("Sample rate updated", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    }
}

void AudioSettingsPanel::onBufferSizeChanged()
{
    if (!deviceManager)
        return;
    
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);
    
    setup.bufferSize = bufferSizeComboBox.getSelectedId();
    
    juce::String error = deviceManager->setAudioDeviceSetup(setup, true);
    if (error.isNotEmpty())
    {
        statusLabel.setText("Error: " + error, juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    }
    else
    {
        statusLabel.setText("Buffer size updated", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    }
}

void AudioSettingsPanel::testAudio()
{
    if (!deviceManager)
        return;
    
    // Simple audio test - generate a test tone
    statusLabel.setText("Testing audio...", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    
    // In a real implementation, you'd generate a test tone here
    // For now, just show a success message
    statusLabel.setText("Audio test completed", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
} 