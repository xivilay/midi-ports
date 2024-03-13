## Description
Small utility plugin that can be used to route midi inputs and outputs.

![image](https://github.com/xivilay/midi-ports/assets/20886280/da4390b3-f53e-47b3-9e90-70d079ed88d2)

## Notes
Generally, audio plugin is not supposed to get access to I/O, since DAW itself should control it. That's why it is not always possible to sync the access correctly (DAW is not exposing it) and direct access may work with issues. Consider use it only as a workaround.

## Bluetooth MIDI

### Windows
**Stability: Low**

`WinRT` midi is supported through `JUCE`.

You need to **pair your midi device manually** first before seeing it in inputs list.

### MacOS
**Stability: High**

`Connect BT Midi Inputs` button brings you to the standard pairing window.

![image](https://github.com/xivilay/midi-ports/assets/20886280/2cce362b-2eea-4207-9bac-3b03ffbf54fd)

A device should be available in inputs list after pairing.