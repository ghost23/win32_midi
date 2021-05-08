# win32_midi

This is more of a proof of concept to show how to connect flutter to a Win32 api via dart:ffi
and get an async callback.

If you're interested, the method in question here is `Win32Midi.openMidiInput(...)` in ./lib/win32_midi.dart
and its counterpart `openMidiInput(...)` in ./windows/win32_midi_plugin.cpp, which is taking
an async callback.

## Getting Started

This project is a starting point for a Flutter
[plug-in package](https://flutter.dev/developing-packages/),
a specialized package that includes platform-specific implementation code for
Android and/or iOS.

For help getting started with Flutter, view our
[online documentation](https://flutter.dev/docs), which offers tutorials,
samples, guidance on mobile development, and a full API reference.

