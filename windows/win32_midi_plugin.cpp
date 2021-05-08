// This must be included before many other Windows headers.
#include <Windows.h>

#include "include/win32_midi/win32_midi_plugin.h"

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <thread>
#include "CallbackManager.h"

#include <stdio.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace {

class Win32MidiPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  Win32MidiPlugin();

  virtual ~Win32MidiPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

// static
void Win32MidiPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "win32_midi",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<Win32MidiPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

Win32MidiPlugin::Win32MidiPlugin() {}

Win32MidiPlugin::~Win32MidiPlugin() {}

void Win32MidiPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void Win32MidiPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  Win32MidiPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

UINT getMidiDeviceIndexes() {
    return midiInGetNumDevs();
}

MIDIINCAPS getMidiInDeviceCapabilities(int i) {
    MIDIINCAPS caps;
    midiInGetDevCaps(i, &caps, 32);
    return caps;
}

HMIDIIN hMidiDevice{ NULL };
Dart_Port commonCallbackPort{ 0 };

void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
    unsigned long values[4];
    values[0] = wMsg;
    values[1] = dwInstance;
    values[2] = dwParam1;
    values[3] = dwParam2;

    callbackToDartInt32Array(commonCallbackPort, 4, values);
    return;
}

MMRESULT openMidiInput(DWORD port, Dart_Port callbackPort) {
    
    commonCallbackPort = callbackPort;
    return midiInOpen(&hMidiDevice, port, (DWORD_PTR)(void*)MidiInProc, 0, CALLBACK_FUNCTION);
}

MMRESULT startMidiInput() {
    return midiInStart(hMidiDevice);
}

MMRESULT stopMidiInput() {
    return midiInStop(hMidiDevice);
}

MMRESULT closeMidiInput() {
    MMRESULT result = midiInClose(hMidiDevice);
    hMidiDevice = NULL;
    return result;
}