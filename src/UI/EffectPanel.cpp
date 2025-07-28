#include "EffectPanel.h"
#include "../AudioProcessor.h"
#include "../EffectProcessor.h"

EffectPanel::EffectPanel(AudioProcessor* processor)
    : audioProcessor(processor)
{
    setupUI();
}

EffectPanel::~EffectPanel()
{
    for (auto* slider : parameterSliders)
        delete slider;
    for (auto* label : parameterLabels)
        delete label;
}

void EffectPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2d2d2d));
}

void EffectPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Top controls area
    auto topArea = bounds.removeFromTop(120);
    
    // Left side - effect type buttons
    auto leftArea = topArea.removeFromLeft(200);
    addDistortionButton.setBounds(leftArea.removeFromTop(25).reduced(5));
    addReverbButton.setBounds(leftArea.removeFromTop(25).reduced(5));
    addDelayButton.setBounds(leftArea.removeFromTop(25).reduced(5));
    addChorusButton.setBounds(leftArea.removeFromTop(25).reduced(5));
    removeEffectButton.setBounds(leftArea.removeFromTop(25).reduced(5));
    
    // Right side - effect selection
    auto rightArea = topArea;
    effectListComboBox.setBounds(rightArea.removeFromTop(25).reduced(5));
    effectEnabledToggle.setBounds(rightArea.removeFromTop(25).reduced(5));
    
    // Parameter controls
    auto paramArea = bounds;
    parameterLabel.setBounds(paramArea.removeFromTop(25));
    
    // Layout parameter sliders in a grid
    int sliderHeight = 60;
    int slidersPerRow = 2;
    int currentRow = 0;
    int currentCol = 0;
    
    for (size_t i = 0; i < parameterSliders.size(); ++i)
    {
        int x = currentCol * (paramArea.getWidth() / slidersPerRow);
        int y = currentRow * sliderHeight;
        
        auto sliderBounds = paramArea.removeFromTop(sliderHeight).reduced(5);
        sliderBounds.setX(x);
        sliderBounds.setWidth(paramArea.getWidth() / slidersPerRow - 10);
        
        if (parameterLabels[i])
            parameterLabels[i]->setBounds(sliderBounds.removeFromTop(20));
        if (parameterSliders[i])
            parameterSliders[i]->setBounds(sliderBounds);
        
        currentCol++;
        if (currentCol >= slidersPerRow)
        {
            currentCol = 0;
            currentRow++;
        }
    }
}

void EffectPanel::setupUI()
{
    // Setup effect buttons
    addDistortionButton.setButtonText("Add Distortion");
    addDistortionButton.onClick = [this]() { addEffect(0); }; // Distortion
    addAndMakeVisible(addDistortionButton);
    
    addReverbButton.setButtonText("Add Reverb");
    addReverbButton.onClick = [this]() { addEffect(1); }; // Reverb
    addAndMakeVisible(addReverbButton);
    
    addDelayButton.setButtonText("Add Delay");
    addDelayButton.onClick = [this]() { addEffect(2); }; // Delay
    addAndMakeVisible(addDelayButton);
    
    addChorusButton.setButtonText("Add Chorus");
    addChorusButton.onClick = [this]() { addEffect(3); }; // Chorus
    addAndMakeVisible(addChorusButton);
    
    removeEffectButton.setButtonText("Remove Effect");
    removeEffectButton.onClick = [this]() { removeSelectedEffect(); };
    addAndMakeVisible(removeEffectButton);
    
    // Setup effect selection
    effectListComboBox.onChange = [this]() { onEffectSelected(); };
    addAndMakeVisible(effectListComboBox);
    
    effectEnabledToggle.setButtonText("Enabled");
    effectEnabledToggle.onClick = [this]() {
        if (audioProcessor)
        {
            int selectedId = effectListComboBox.getSelectedId();
            if (selectedId > 0)
            {
                audioProcessor->setEffectEnabled(selectedId, effectEnabledToggle.getToggleState());
            }
        }
    };
    addAndMakeVisible(effectEnabledToggle);
    
    // Setup parameter label
    parameterLabel.setText("Parameters:", juce::dontSendNotification);
    parameterLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(parameterLabel);
    
    updateEffectList();
}

void EffectPanel::addEffect(int effectType)
{
    if (!audioProcessor)
        return;
    
    int effectId = audioProcessor->addEffect(effectType);
    if (effectId > 0)
    {
        updateEffectList();
        effectListComboBox.setSelectedId(effectId);
        onEffectSelected();
    }
}

void EffectPanel::removeSelectedEffect()
{
    if (!audioProcessor)
        return;
    
    int selectedId = effectListComboBox.getSelectedId();
    if (selectedId > 0)
    {
        audioProcessor->removeEffect(selectedId);
        updateEffectList();
        updateParameterControls();
    }
}

void EffectPanel::updateEffectList()
{
    if (!audioProcessor)
        return;
    
    effectItems.clear();
    effectListComboBox.clear();
    
    // Get effects from the effect processor
    auto effectProcessor = audioProcessor->getEffectProcessor();
    if (effectProcessor)
    {
        const auto& effects = effectProcessor->getEffects();
        for (const auto& effect : effects)
        {
            EffectItem item;
            item.id = effect.id;
            item.name = getEffectTypeName(static_cast<int>(effect.type));
            item.enabled = effect.enabled;
            
            effectItems.push_back(item);
            effectListComboBox.addItem(item.name, effect.id);
        }
    }
    
    if (effectListComboBox.getNumItems() > 0)
        effectListComboBox.setSelectedId(0);
}

void EffectPanel::updateParameterControls()
{
    // Clear existing controls
    for (auto* slider : parameterSliders)
    {
        removeChildComponent(slider);
        delete slider;
    }
    parameterSliders.clear();
    
    for (auto* label : parameterLabels)
    {
        removeChildComponent(label);
        delete label;
    }
    parameterLabels.clear();
    
    if (!audioProcessor)
        return;
    
    int selectedId = effectListComboBox.getSelectedId();
    if (selectedId <= 0)
        return;
    
    auto effectProcessor = audioProcessor->getEffectProcessor();
    if (!effectProcessor)
        return;
    
    auto* effect = effectProcessor->getEffect(selectedId);
    if (!effect || !effect->effect)
        return;
    
    // Create parameter controls
    int numParams = effect->effect->getNumParameters();
    for (int i = 0; i < numParams; ++i)
    {
        // Create label
        auto* label = new juce::Label();
        label->setText(effect->effect->getParameterName(i), juce::dontSendNotification);
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
        parameterLabels.push_back(label);
        
        // Create slider
        auto* slider = new juce::Slider();
        slider->setSliderStyle(juce::Slider::LinearHorizontal);
        slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        slider->setRange(effect->effect->getParameterMinValue(i), 
                        effect->effect->getParameterMaxValue(i), 0.01);
        slider->setValue(effect->effect->getParameter(i));
        slider->onValueChange = [this, i, slider]() {
            onParameterChanged(i, static_cast<float>(slider->getValue()));
        };
        addAndMakeVisible(slider);
        parameterSliders.push_back(slider);
    }
    
    resized();
}

void EffectPanel::onEffectSelected()
{
    if (!audioProcessor)
        return;
    
    int selectedId = effectListComboBox.getSelectedId();
    if (selectedId > 0)
    {
        effectEnabledToggle.setToggleState(audioProcessor->isEffectEnabled(selectedId), 
                                         juce::dontSendNotification);
        updateParameterControls();
    }
}

void EffectPanel::onParameterChanged(int parameterId, float value)
{
    if (!audioProcessor)
        return;
    
    int selectedId = effectListComboBox.getSelectedId();
    if (selectedId > 0)
    {
        audioProcessor->setEffectParameter(selectedId, parameterId, value);
    }
}

juce::String EffectPanel::getEffectTypeName(int effectType)
{
    switch (effectType)
    {
        case 0: return "Distortion";
        case 1: return "Reverb";
        case 2: return "Delay";
        case 3: return "Chorus";
        default: return "Unknown";
    }
} 