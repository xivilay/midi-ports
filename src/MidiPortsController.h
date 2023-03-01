#pragma once

#include <JuceHeader.h>

class MidiPortsController {
   public:
    MidiPortsController(MidiMessageCollector* c, AudioProcessor& p) {
        collector = c;

        AudioDeviceManager::AudioDeviceSetup s;
        s.sampleRate = p.getSampleRate();
        s.bufferSize = p.getBlockSize();

        deviceManager.initialise(0, 1, nullptr, false, "", &s);
    }
    ~MidiPortsController() {}

    bool setMidiInput(String id) {
        auto midiInputs = MidiInput::getAvailableDevices();
        for (auto input : midiInputs) {
            if (id == input.identifier) {
                if (midiInputIds.contains(id)) {
                    deviceManager.setMidiInputDeviceEnabled(id, false);
                    deviceManager.removeMidiInputDeviceCallback(id, collector);
                    midiInputIds.remove(midiInputIds.indexOf(id));
                    return false;
                }

                deviceManager.setMidiInputDeviceEnabled(id, true);
                if (deviceManager.isMidiInputDeviceEnabled(id)) {
                    deviceManager.addMidiInputDeviceCallback(id, collector);
                    midiInputIds.add(id);
                    return true;
                }
                break;
            }
        }
        return false;
    }

    bool setMidiOutput(String id) {
        auto midiOutputs = MidiOutput::getAvailableDevices();
        for (auto output : midiOutputs) {
            if (id == output.identifier) {
                if (midiOutputIds.contains(id)) {
                    midiOutputIds.remove(midiOutputIds.indexOf(id));
                    auto device = MidiOutput::openDevice(id);
                    if (device.get() != nullptr) {
                        device.reset();
                    }
                    return false;
                }
                auto device = MidiOutput::openDevice(id);
                if (device.get() != nullptr) {
                    midiOutputIds.add(id);
                    return true;
                }
                break;
            }
        }
        return false;
    }

    void processBlock(MidiBuffer& midiMessages, int numSamples) {
        // midiMessages.clear(); // filter host messages

        const ScopedLock sl(lock);

        collector->removeNextBlockOfMessages(midiMessages, numSamples);

        MidiBuffer nextMessages;

        for (const auto metadata : midiMessages) {
            nextMessages.addEvent(metadata.getMessage(), metadata.samplePosition);
        }

        midiMessages.swapWith(nextMessages);

        for (auto id : midiOutputIds) {
            auto device = MidiOutput::openDevice(id);
            if (device != nullptr && device.get() != nullptr) {
                device.get()->sendBlockOfMessagesNow(midiMessages);
            }
        }
    }

    int getMidiInputsCount() { return midiInputIds.size(); }
    int getMidiOutputsCount() { return midiOutputIds.size(); }

    auto getMidiInputIds() { return midiInputIds; }
    auto getMidiOutputIds() { return midiOutputIds; }

    static const int HOST_INPUT_ID = 1;

   private:
    AudioDeviceManager deviceManager;
    SortedSet<String> midiInputIds;
    SortedSet<String> midiOutputIds;

    MidiMessageCollector* collector;

    CriticalSection lock;
};