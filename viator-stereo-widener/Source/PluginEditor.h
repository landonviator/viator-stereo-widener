#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ViatorstereowidenerAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    ViatorstereowidenerAudioProcessorEditor (ViatorstereowidenerAudioProcessor&);
    ~ViatorstereowidenerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ViatorstereowidenerAudioProcessor& audioProcessor;
    
    // cpu
    juce::Label _cpuLabel;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViatorstereowidenerAudioProcessorEditor)
};
