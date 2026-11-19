# Kanako Voice Changer

Kanako Voice Changer is a planned standalone Windows desktop application for real-time voice changing. The goal is to provide selectable female and male voice presets in one integrated Kanako application, without a separate client/demo application, paid API, subscription, or premium feature lock.

> **Project status: Initial repository setup.** This repository currently contains project documentation and structure only. A working audio engine, voice models, installer, and runtime tests have not yet been added or verified.

## Project goals

- One Windows application named **Kanako Voice Changer**.
- Selectable female and male voice presets.
- Real-time microphone processing, with clear input/output device selection.
- A usable audio-level meter and reliable start/stop controls.
- Local processing by default; no paid cloud voice API.
- No subscription or premium-locked features.
- A professional installer and clear installation/uninstallation experience.
- Transparent dependency and model licensing.

## Repository layout

- `app/` — desktop application and user interface.
- `audio/` — audio capture, playback, routing, and device handling.
- `voice-engine/` — voice conversion and inference integration.
- `voice-models/` — model manifests and download/install metadata. Large model binaries should not be committed unless their licenses and distribution terms permit it.
- `soundboard/` — optional soundboard functionality.
- `effects/` — optional audio effects.
- `drivers/` — virtual audio driver integration notes and installer assets, if required.
- `installer/` — Windows packaging and installer configuration.
- `tests/` — automated and manual test plans.
- `docs/` — architecture, setup, licensing, and release documentation.

## Development principles

1. Do not represent a feature as complete until its implementation and tests have been verified.
2. Prefer free, locally runnable dependencies; review each dependency and model license before use or redistribution.
3. Keep the voice engine integrated behind the Kanako application. Do not ship a separate demo/client product.
4. Do not bundle third-party binaries, models, or drivers without checking their redistribution terms.
5. Keep user recordings and microphone audio local unless the user explicitly chooses otherwise.
6. Do not change voice-processing quality settings or add hidden paid dependencies.

## Current status

| Area | Status |
|---|---|
| Public GitHub repository | Set up |
| Project overview and documentation | Initial setup |
| Desktop UI | Not implemented/verified in this repository |
| Audio capture and routing | Not implemented/verified |
| Voice conversion engine | Not implemented/verified |
| Female/male voice presets | Not implemented/verified |
| Windows installer | Not implemented/verified |
| Automated build and tests | Not implemented/verified |

## Next steps

See [the roadmap](docs/ROADMAP.md) and [architecture proposal](docs/ARCHITECTURE.md). The implementation stack and exact feature scope must be confirmed before adding engine dependencies and downloadable model assets.

## License

No software license has been added yet. Public visibility does not by itself grant permission to reuse, modify, or redistribute this code. A license will be selected and added before the project is presented as open source.
