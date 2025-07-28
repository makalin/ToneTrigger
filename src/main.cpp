#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "MainComponent.h"

class ToneTriggerApplication : public juce::JUCEApplication
{
public:
    ToneTriggerApplication() = default;

    const juce::String getApplicationName() override { return "ToneTrigger"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
        
        mainWindow.reset(new juce::DocumentWindow("ToneTrigger",
            juce::Colours::darkgrey,
            juce::DocumentWindow::allButtons));

        mainWindow->setUsingNativeTitleBar(true);
        mainWindow->setContentOwned(new MainComponent(), true);
        mainWindow->setResizable(true, true);
        mainWindow->setResizeLimits(800, 600, 1920, 1080);
        mainWindow->centreWithSize(1200, 800);
        mainWindow->setVisible(true);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
    }

private:
    std::unique_ptr<juce::DocumentWindow> mainWindow;
};

START_JUCE_APPLICATION(ToneTriggerApplication) 