#pragma once

#include <JuceHeader.h>

#include "CustomEditor.h"
#include "MidiPortsController.h"

class PluginProcessor : public AudioProcessor {
   public:
    PluginProcessor() : AudioProcessor(BusesProperties()), collector(), controller(&collector) {}

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {
        buffer.clear();

        // midiMessages.clear(); // filter host messages

        collector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());

        MidiBuffer nextMessages;

        for (const auto metadata : midiMessages) {
            auto message = metadata.getMessage();
            const auto time = metadata.samplePosition;
            nextMessages.addEvent(message, time);
        }

        midiMessages.clear();
        midiMessages.swapWith(nextMessages);
    }

    const String getName() const override { return ProjectInfo::projectName; }

    AudioProcessorEditor *createEditor() override { return new CustomEditor(*this, &controller); };

    bool hasEditor() const override { return true; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName(int, const String &) override {}

    double getTailLengthSeconds() const override { return 0; }

    void getStateInformation(MemoryBlock &) override {}
    void setStateInformation(const void *, int) override {}

    void setMidiInput(String id) {}

   private:
    MidiPortsController controller;
    MidiMessageCollector collector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
