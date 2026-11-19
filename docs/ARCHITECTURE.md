# Kanako Voice Changer — Architecture Decision

This document records the current implementation direction. It does not claim that the application or audio processing is already working.

## Product shape

Kanako is one integrated Windows desktop application. The user must not need to launch a separate Voice Changer Client Demo, browser client, or visible helper application. Internal libraries are implementation details bundled with Kanako.

## Product scope

- Local, real-time microphone voice effects.
- Built-in selectable female-style and male-style presets.
- One-click start/stop, microphone and speaker selection, input/output meters, and clear device errors.
- No AI model training, AI inference engine, cloud voice API, subscription, or premium feature lock.
- No copying of VoiceWave code, assets, or binaries.

The initial presets will use conventional digital signal processing (pitch shifting and formant/timbre adjustment). They are voice effects, not guaranteed transformations into a natural-sounding different person's voice. Preset quality must be judged by listening tests on real Windows hardware.

## Selected technical direction

| Layer | Direction | Reason |
|---|---|---|
| Desktop UI | Qt 6 Widgets, C++ | Native desktop window and direct integration with the audio engine; no separate web client. |
| Build | CMake, C++17 or later | Standard Windows-native build workflow and repeatable dependency configuration. |
| Audio I/O | PortAudio | Cross-device capture/playback API with a real-time callback model. |
| Pitch/time processing | Signalsmith Stretch | C++ pitch/time library with pitch transpose and formant-compensation controls. |
| Presets | App-owned configuration | Female-style and male-style values remain editable without downloading models. |
| Installer | To be selected after a working Windows build | Installer choice must support a single app, dependency notices, upgrade, repair, and uninstall. |

These are source-backed candidate selections, not yet validated in a Kanako build. Exact versions, Windows toolchain, callback design, buffer sizes, and device support must be pinned and tested before release.

## Audio pipeline

1. Enumerate Windows audio input and output devices.
2. Open the selected input and output streams at a supported sample rate.
3. Capture microphone frames in the audio callback.
4. Process frames using preallocated buffers and the selected DSP preset.
5. Send processed frames to the selected output stream.
6. Publish meter values to the UI through a thread-safe mechanism; never update Qt widgets directly from the audio callback.
7. Stop and release streams safely when the user stops, changes devices, or closes the app.

The callback must avoid blocking operations, file/network access, dynamic allocation, and UI calls. Device errors and stream restarts must be handled outside the callback.

## Initial preset design

The first preset set will include:
- Female-style: moderate upward pitch shift with a separately tuned formant/timbre adjustment.
- Male-style: moderate downward pitch shift with a separately tuned formant/timbre adjustment.
- Neutral/bypass: original microphone signal for comparison and troubleshooting.

Exact semitone and formant values are tuning parameters, not final values. They must be selected through listening tests to avoid chipmunk, metallic, robotic, or garbled speech.

## Data and privacy

- Microphone audio is processed locally.
- No voice recordings are uploaded.
- No telemetry unless it is clearly disclosed and optional.
- Any diagnostic recording is saved only when the user explicitly starts recording.

## Licensing and redistribution

- Kanako application source is MIT licensed.
- Qt 6 is intended to be used under its applicable open-source LGPL terms with the required notices and compliant dynamic linking; verify the exact Qt modules and obligations before packaging.
- PortAudio and Signalsmith Stretch are MIT-licensed upstream projects. Include their copyright and license notices when bundled.
- Keep a version-pinned dependency inventory and ship third-party notices with every release.
- Do not bundle any model, driver, codec, or runtime until its license and redistribution terms are verified.

## Not yet verified

- No Kanako executable has been built.
- No real microphone/speaker stream has been tested through the proposed stack.
- No latency, dropout, clipping, or voice-quality results exist yet.
- No installer or virtual microphone driver has been selected.
