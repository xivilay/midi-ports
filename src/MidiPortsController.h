#pragma once

#include <JuceHeader.h>

class MidiPortsController {
   public:
    MidiPortsController(MidiMessageCollector* c) {
        collector = c;

        deviceManager.initialise(0, 1, nullptr, true, "", nullptr);
        deviceManager.addMidiInputDeviceCallback({}, collector);
    }
    ~MidiPortsController() {}

    AudioDeviceManager* getDeviceManager() { return &deviceManager; }

    bool setMidiInput(String id) {
        auto midiInputs = MidiInput::getAvailableDevices();
        for (auto input : midiInputs) {
            if (id == input.identifier) {
                if (midiInputIds.contains(id)) {
                    deviceManager.setMidiInputDeviceEnabled(id, false);
                    midiInputIds.remove(midiInputIds.indexOf(id));
                    return false;
                }
                deviceManager.setMidiInputDeviceEnabled(id, true);
                if (deviceManager.isMidiInputDeviceEnabled(id)) {
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

        collector->removeNextBlockOfMessages(midiMessages, numSamples);

        MidiBuffer nextMessages;

        for (const auto metadata : midiMessages) {
            auto message = metadata.getMessage();
            const auto time = metadata.samplePosition;
            nextMessages.addEvent(message, time);
        }

        midiMessages.clear();
        midiMessages.swapWith(nextMessages);

        for (auto id : midiOutputIds) {
            const ScopedLock sl (midiCallbackLock);
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

    CriticalSection midiCallbackLock;
};