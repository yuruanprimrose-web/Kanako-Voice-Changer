# Kanako Voice Changer

Kanako is one integrated Windows desktop voice changer using local, non-AI digital signal processing. The application includes Neutral/Female/Male-style presets, adjustable Pitch and Formant controls, microphone/output gain, and WAV recording.

## Product commitments

- One Kanako application — no separate Voice Changer Client Demo.
- Non-AI DSP; no model training, AI inference engine, or cloud voice upload.
- No paid cloud API, subscription, or premium feature lock.
- Local microphone processing and user-controlled WAV recording.
- No copied proprietary VoiceWave code, assets, or binaries.
- Kanako's own source is MIT licensed. Third-party components retain their own licenses.

## Current verified status

The current working baseline is **Build #52**.

- [x] Windows x64 GitHub Actions build and portable artifact completed successfully (Build #52, commit `ea359548ae635ebbb8a1e060c0649478ef2b4380`).
- [x] Neutral, Female-style, and Male-style presets — user-tested.
- [x] Adjustable Pitch and Formant sliders — user-tested.
- [x] Microphone and output gain behavior — user-tested.
- [x] WAV recording pipeline automated smoke test — passed in CI.
- [x] WAV recording with audible voice — explicitly confirmed by the user on Windows.
- [x] Optional VB-CABLE output routing prototype is implemented in source with a separate PortAudio output stream and bounded fan-out buffer; automated buffer tests are included in CI.
- [ ] Real Windows virtual microphone routing, Discord/Zoom/OBS/YouTube/Facebook compatibility, and long-duration stability — pending manual testing.
- [ ] Discord, Zoom, OBS, YouTube, and Facebook input compatibility — not yet tested.
- [ ] Production installer, upgrade, repair, and uninstall flow — not yet completed.

**Important:** CI success confirms the automated build/tests only. Real-device and third-party application compatibility must be tested on Windows before being claimed.

## Virtual microphone investigation

The next development priority is a Windows recording endpoint that exposes Kanako's processed voice to other applications. Driver options, redistribution terms, routing design, signing requirements, and test gates are documented in [Virtual Microphone Plan](docs/VIRTUAL_MICROPHONE_PLAN.md).

No third-party driver is bundled. The user must install VB-CABLE separately from the official VB-Audio site for the first hardware test. The prototype is not yet verified on a real Windows endpoint; no application compatibility is claimed. The plan keeps a future Kanako-owned driver possible.

## Build configuration

- C++17
- Qt 6.8.3 Widgets
- CMake
- PortAudio 19.7.0
- Signalsmith Stretch 1.4.0

See [architecture](docs/ARCHITECTURE.md), [roadmap](docs/ROADMAP.md), [testing notes](docs/TESTING.md), [third-party license inventory](docs/THIRD_PARTY_LICENSES.md), and [virtual microphone plan](docs/VIRTUAL_MICROPHONE_PLAN.md).

## Repository layout

- `app/` — integrated desktop application source
- `voice-engine/` — non-AI DSP processing
- `drivers/` — reserved for future driver integration; no Kanako virtual driver is implemented yet
- `installer/` — future Windows packaging
- `tests/` — automated and manual test assets
- `docs/` — architecture, roadmap, testing, and license records

## Testing the current portable build

Download the `Kanako-Voice-Changer-Windows-x64` artifact from the latest successful Windows build in GitHub Actions. Extract the entire ZIP to a normal folder and run `Kanako Voice Changer.exe` from inside that extracted folder. Keep the Qt runtime files and plugin folders beside the EXE.

Use headphones during live monitoring to reduce feedback. Test Neutral first, then Female-style and Male-style at a safe volume. WAV recording can be started while the voice changer is running; stop recording to choose where to save the file.

## License

Kanako's own source is MIT licensed. Third-party components retain their own licenses; see [LICENSE](LICENSE) and [docs/THIRD_PARTY_LICENSES.md](docs/THIRD_PARTY_LICENSES.md).
