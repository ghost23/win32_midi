import 'dart:ffi';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:win32_midi/win32_midi.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: Main(),
    );
  }
}

class Main extends StatefulWidget {
  @override
  _MainState createState() => _MainState();
}

class _MainState extends State<Main> {
  Win32Midi win32midi = Win32Midi();

  int choosenMidiDevice = -1;
  MIDIINCAPS? selectedMidiDeviceCaps;

  List<String> messages = [];

  @override
  void initState() {
    super.initState();
  }

  Future<void> chooseMidiDevice(BuildContext context) async {
    int devices = win32midi.getMidiDeviceIndexes();
    int? newDevice = await showDialog<int>(
        context: context,
        builder: (BuildContext context) {
          return SimpleDialog(
            title: const Text('Select Midi Device'),
            children: <Widget>[
              for ( var i = 0; i < devices; i++ )
                SimpleDialogOption(
                  onPressed: () { Navigator.pop(context, i); },
                  child: Text(win32midi.getMidiInDeviceCapabilities(i).szPname),
                ),
            ],
          );
        }
    );

    if(newDevice != null) {
      setState(() {
        choosenMidiDevice = newDevice;
        selectedMidiDeviceCaps = win32midi.getMidiInDeviceCapabilities(choosenMidiDevice);
      });

      win32midi.openMidiInput(choosenMidiDevice, onMidiMessage);
      win32midi.startMidiInput();
    }
  }

  void onMidiMessage(MidiMessage message) {
    setState(() {
      messages.add('${message.timestamp}: $message');
    });
  }

  void disconnect() {
    win32midi.stopMidiInput();
    win32midi.closeMidiInput();
    if(mounted) {
      setState(() {
        choosenMidiDevice = -1;
        selectedMidiDeviceCaps = null;
      });
    }
  }

  @override
  void dispose() {
    super.dispose();
    disconnect();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(choosenMidiDevice < 0 ? 'Win Midi' : 'Win Midi: ${selectedMidiDeviceCaps!.szPname}'),
        actions: [
          IconButton( icon: Icon(Icons.phonelink), onPressed: () => chooseMidiDevice(context), tooltip: 'MIDI'),
        ],
      ),
      floatingActionButton: choosenMidiDevice >= 0 ? FloatingActionButton(onPressed: disconnect, child: Icon(Icons.close), backgroundColor: Colors.red,) : null,
      body: Padding(
        padding: const EdgeInsets.all(24.0),
        child: SingleChildScrollView(
          child: Text(messages.join('\n')),
        ),
      )
    );
  }
}