# Kanako Voice Changer

Kanako is being developed as one integrated Windows desktop voice changer. The goal is to let a user select built-in female-style or male-style voice effects and process microphone audio locally.

## Product commitments

- One Kanako application — no separate Voice Changer Client Demo.
- Non-AI digital signal processing for the initial voice presets.
- No paid cloud API, subscription, or premium feature lock.
- Local microphone processing; no voice upload.
- No copied proprietary VoiceWave code, assets, or binaries.
- Free and open-source Kanako source under the MIT License.

## Current development status

The repository now contains the Qt 6 desktop window source, CMake configuration, Windows CI workflow, PortAudio device enumeration, and a microphone-to-output passthrough implementation with an input-level meter.

**This is not a finished voice changer yet.** The passthrough code has been committed but has not yet passed a Windows build or real microphone/speaker test. Pitch/formant processing and working female-style/male-style presets are not connected; the preset selector remains disabled. Installer and release testing are also pending.

The Windows CI workflow is intended to compile the current source on GitHub Actions. A successful build must be confirmed from an actual workflow run before it is reported as PASS.

## Planned technology

- C++17 or later
- Qt 6 Widgets for the integrated desktop UI
- CMake for repeatable builds
- PortAudio for audio device access and real-time input/output
- Signalsmith Stretch for pitch/time shifting and formant compensation

Exact dependency versions and redistribution notices must be pinned and checked before packaging. See [the architecture decision](docs/ARCHITECTURE.md), [roadmap](docs/ROADMAP.md), and [third-party license inventory](docs/THIRD_PARTY_LICENSES.md).

## Build direction

The initial Windows build is configured for Visual Studio 2022 x64 and Qt 6.8.3. See the GitHub Actions workflow at `.github/workflows/windows-build.yml`. A local developer setup guide will be added after the first CI build is confirmed.

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

## License

Kanako's own source is MIT licensed. Third-party components retain their own licenses; see [LICENSE](LICENSE) and [docs/THIRD_PARTY_LICENSES.md](docs/THIRD_PARTY_LICENSES.md).
