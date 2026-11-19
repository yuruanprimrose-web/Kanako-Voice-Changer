# Kanako Voice Changer

Kanako is being developed as one integrated Windows desktop voice changer. The current development build provides microphone and output device selection, live microphone monitoring, an input-level meter, and initial non-AI pitch/formant presets.

## Product commitments

- One Kanako application — no separate Voice Changer Client Demo.
- Non-AI digital signal processing for the initial voice presets.
- No paid cloud API, subscription, or premium feature lock.
- Local microphone processing; no voice upload.
- No copied proprietary VoiceWave code, assets, or binaries.
- Kanako's own source is MIT licensed. Third-party components retain their own licenses.

## Current verified status

- [x] Qt 6 desktop application source is present.
- [x] PortAudio microphone and output device enumeration is present.
- [x] Microphone-to-output audio callback is present.
- [x] Input-level meter and start/stop controls are present.
- [x] Neutral, Female-style, and Male-style DSP preset selections are connected to the processing code.
- [x] GitHub Actions Windows x64 build completed successfully (run #22).
- [x] A portable Windows x64 build artifact was uploaded by that run.
- [ ] The downloaded application has not yet been launched and tested on a user's Windows PC.
- [ ] Real microphone/speaker behavior, voice quality, latency, and stability have not yet been verified on target hardware.
- [ ] A proper Windows installer, upgrade flow, and uninstall flow are not yet available.

**Important:** A successful CI compile proves that the source compiled and the portable build folder was produced. It does not prove real-device audio works correctly or that the voice presets sound natural. Treat this as an early development build, not a finished release.

## Build configuration

- C++17
- Qt 6.8.3 Widgets
- CMake
- PortAudio 19.7.0 for audio device access
- Signalsmith Stretch 1.4.0 for pitch/time and formant processing

Exact dependency versions and redistribution notices must be checked before packaging. See [the architecture decision](docs/ARCHITECTURE.md), [roadmap](docs/ROADMAP.md), and [third-party license inventory](docs/THIRD_PARTY_LICENSES.md).

## Repository layout

- `app/` — integrated desktop application source
- `audio/` — audio pipeline and device handling
- `voice-engine/` — non-AI DSP processing
- `voice-models/` — reserved; no AI models are planned for the initial product
- `soundboard/` — optional soundboard feature
- `effects/` — audio effects and presets
- `drivers/` — optional driver integration, subject to licensing
- `installer/` — Windows packaging
- `tests/` — automated and manual test assets
- `docs/` — architecture, roadmap, testing, and license records

## Testing the current portable build

Download the `Kanako-Voice-Changer-Windows-x64` artifact from the latest successful Windows build in GitHub Actions. Extract the entire ZIP to a normal folder and run `Kanako Voice Changer.exe` from inside that extracted folder. Do not move the EXE out by itself because the Qt runtime files and plugin folders must stay beside it.

For the first test, select your microphone and speaker, leave the preset on Neutral / Bypass, and press Start voice changer. Use headphones to reduce feedback. Then test Female-style and Male-style briefly at a safe volume. Stop the audio before changing devices. Report any error text and whether you hear clean audio, altered audio, or silence.

## License

Kanako's own source is MIT licensed. Third-party components retain their own licenses; see [LICENSE](LICENSE) and [docs/THIRD_PARTY_LICENSES.md](docs/THIRD_PARTY_LICENSES.md).
