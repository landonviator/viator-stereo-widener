#pragma once
#include <JuceHeader.h>
#include "globals/Parameters.h"

class ViatorstereowidenerAudioProcessor  : public juce::AudioProcessor
, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ViatorstereowidenerAudioProcessor();
    ~ViatorstereowidenerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // parameters
    ViatorParameters::Params _parameterMap;
    juce::AudioProcessorValueTreeState _treeState;
    
    float getCPULoad();
    
private:
    
    // dsp
    juce::dsp::ProcessSpec m_spec;
    juce::OwnedArray<viator_dsp::SVFilter<float>> _filters;
    juce::AudioProcessLoadMeasurer _cpuMeasureModule;
    std::atomic<float> _cpuLoad;
    juce::OwnedArray<viator_dsp::FastFilter<float>> _filterBank;
    
    // parameters
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    using Parameter = juce::AudioProcessorValueTreeState::Parameter;
    static juce::String valueToTextFunction(float x) { return juce::String(static_cast<int>(x)); }
    static float textToValueFunction(const juce::String& str) { return str.getFloatValue(); }
    void updateParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViatorstereowidenerAudioProcessor)
};
