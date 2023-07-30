/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ViatorstereowidenerAudioProcessorEditor::ViatorstereowidenerAudioProcessorEditor (ViatorstereowidenerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // cpu label
    addAndMakeVisible(_cpuLabel);
    _cpuLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    _cpuLabel.setFont(juce::Font("Helvetica", 16.0f, juce::Font::FontStyleFlags::bold));
    _cpuLabel.setJustificationType(juce::Justification::centred);
    
    // window
    viator_utils::PluginWindow::setPluginWindowSize(0, 0, *this, 1.0, 0.5);
    
    startTimerHz(4);
}

ViatorstereowidenerAudioProcessorEditor::~ViatorstereowidenerAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void ViatorstereowidenerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB(44, 48, 69));
}

void ViatorstereowidenerAudioProcessorEditor::resized()
{
    _cpuLabel.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() * 0.1, getWidth() * 0.1));
}

void ViatorstereowidenerAudioProcessorEditor::timerCallback()
{
    _cpuLabel.setText(juce::String(std::round(audioProcessor.getCPULoad()), 0) + " %", juce::dontSendNotification);
    repaint();
}
