#pragma once

#include <JuceHeader.h>

#include "MidiPortsController.h"

class CustomEditor : public AudioProcessorEditor {
   public:
    CustomEditor(AudioProcessor& proc, MidiPortsController* c) : AudioProcessorEditor(proc) {
        setSize(editorWidth, editorHeight);

        controller = c;

        updateSelectedCountText();

        addAndMakeVisible(midiInputSelector);
        addAndMakeVisible(midiOutputSelector);

        addAndMakeVisible(settingsButton);

        settingsButton.onClick = [&] { showAudioSettingsDialog(); };

        midiInputSelector.onClick = [&] {
            fillInputSelector();
            midiInputMenu.showMenuAsync(
                PopupMenu::Options{}.withTargetComponent(midiInputSelector).withStandardItemHeight(50), [&](int id) {
                    auto midiInputs = MidiInput::getAvailableDevices();
                    for (auto input : midiInputs) {
                        if (id == getIntId(input.identifier)) {
                            controller->setMidiInput(input.identifier);
                            updateSelectedCountText();
                        }
                    }
                });
        };

        midiOutputSelector.onClick = [&] {
            fillOutputSelector();
            midiOutputMenu.showMenuAsync(
                PopupMenu::Options{}.withTargetComponent(midiOutputSelector).withStandardItemHeight(50), [&](int id) {
                    auto midiOutputs = MidiOutput::getAvailableDevices();
                    for (auto output : midiOutputs) {
                        if (id == getIntId(output.identifier)) {
                            controller->setMidiOutput(output.identifier);
                            updateSelectedCountText();
                        }
                    }
                });
        };

        if (BluetoothMidiDevicePairingDialogue::isAvailable()) {
            addAndMakeVisible(btButton);
            btButton.onClick = [] {
                RuntimePermissions::request(RuntimePermissions::bluetoothMidi, [](bool wasGranted) {
                    if (wasGranted) BluetoothMidiDevicePairingDialogue::open();
                });
            };
        }

        // by some reason it only returns devices after second attempt
        MidiInput::getAvailableDevices();
    }
    ~CustomEditor() {}

    void resized() {
        auto r = getLocalBounds();
        auto currentWidth = r.getWidth();

        const int componentHeight = 50;

        midiInputSelector.setBounds(0, 0, currentWidth / 2, componentHeight);
        midiOutputSelector.setBounds(currentWidth / 2, 0, currentWidth / 2, componentHeight);
        settingsButton.setBounds(0, componentHeight, currentWidth / 2, componentHeight);
        btButton.setBounds(currentWidth / 2, componentHeight, currentWidth / 2, componentHeight);
    }
    void paint(Graphics& g) { g.fillAll(Colours::transparentWhite); }

    int getIntId(String id) { return DefaultHashFunctions::generateHash(id, 100) + 1; }

    void fillInputSelector() {
        midiInputMenu.clear();
        updateSelectedCountText();
        midiInputMenu.addItem(1, "[Input From Plugin Host]", false, true);
        midiInputMenu.addSeparator();

        auto midiInputIds = controller->getMidiInputIds();
        auto midiInputs = MidiInput::getAvailableDevices();
        for (auto input : midiInputs) {
            bool state = midiInputIds.indexOf(input.identifier) != -1;
            midiInputMenu.addItem(getIntId(input.identifier), input.name, true, state);
        }
    }

    void fillOutputSelector() {
        midiOutputMenu.clear();
        updateSelectedCountText();
        midiOutputMenu.addItem(1, "[Output To Plugin Host]", false, true);
        midiOutputMenu.addSeparator();

        auto midiOutputIds = controller->getMidiOutputIds();
        auto midiOutputs = MidiOutput::getAvailableDevices();
        for (auto output : midiOutputs) {
            if (!output.name.contains("Microsoft GS")) {
                bool state = midiOutputIds.indexOf(output.identifier) != -1;
                midiOutputMenu.addItem(getIntId(output.identifier), output.name, true, state);
            }
        }
    }

    void updateSelectedCountText() {
        auto count = controller->getMidiInputsCount();
        if (count != 0) {
            midiInputSelector.setButtonText(String(count) + " input(s) selected");
        } else {
            midiInputSelector.setButtonText("Select Midi Input");
        }
        count = controller->getMidiOutputsCount();
        if (count != 0) {
            midiOutputSelector.setButtonText(String(count) + " output(s) selected");
        } else {
            midiOutputSelector.setButtonText("Select Midi Output");
        }
    }

   private:
    void showAudioSettingsDialog() {
        DialogWindow::LaunchOptions o;

        auto dm = std::make_unique<AudioDeviceSelectorComponent>(*(controller->getDeviceManager()), 0, 0, 0, 0, false,
                                                                 false, false, true);

        dm->setSize(500, 550);
        o.content.setOwned(dm.release());

        o.dialogTitle = TRANS("Audio Devices");
        o.dialogBackgroundColour = o.content->getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
        o.escapeKeyTriggersCloseButton = true;
        o.useNativeTitleBar = true;
        o.resizable = true;

        o.launchAsync();
    }

    int editorWidth = 400;
    int editorHeight = 100;

    TextButton midiInputSelector;
    TextButton midiOutputSelector;

    PopupMenu midiInputMenu;
    PopupMenu midiOutputMenu;

    TextButton btButton{"Connect Bluetooth Midi Inputs"};
    TextButton settingsButton{"Settings"};

    MidiPortsController* controller;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomEditor)
};
