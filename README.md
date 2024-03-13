## Description
Small utility plugin that can be used to route midi inputs and outputs.

![image](https://github.com/xivilay/midi-ports/assets/20886280/da4390b3-f53e-47b3-9e90-70d079ed88d2)

## Notes
Generally, audio plugin is not supposed to get access to I/O, since DAW itself should control it. That's why ist is not always possible to sync the access correctly (DAW is not exposing it) and direct access may work with issues. Consider use it only as a workaround.

## Bluetooth MIDI

### Windows

`WinRT` midi is supported through `JUCE`.

You need to **pair your midi device manually** first before seeing it in inputs list.

### MacOS
`Bluetooth` button brings you standard pairing window.

A device should be available in inputs list after pairing.