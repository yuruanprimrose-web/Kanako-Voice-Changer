# Architecture Proposal

This document records the intended boundaries. It is a proposal, not a claim that these components already exist.

## Product shape

Kanako is one Windows desktop application. The user-facing UI, audio device controls, preset selector, installation experience, and diagnostics belong to Kanako. Internal engine processes may be packaged as private implementation components, but they must not appear as a separate user-facing demo/client product.

## Proposed component boundaries

1. **Desktop UI** — device selection, input meter, preset selection, start/stop, status, and settings.
2. **Audio I/O layer** — microphone capture, output playback, buffering, device enumeration, and recovery.
3. **Voice processing adapter** — a stable interface between the audio pipeline and whichever conversion engine is selected.
4. **Model manager** — model metadata, download/import, integrity validation, and license notices.
5. **Installer/update layer** — installs and updates the integrated application and its required runtime components.

## Decisions still required

- Desktop framework and language.
- Audio backend and supported sample rates/buffer sizes.
- Voice conversion engine and model format.
- CPU-only baseline and optional GPU acceleration strategy.
- Whether a virtual microphone driver is necessary for the first release.
- Installer technology and update mechanism.

These decisions should be made after checking current engine compatibility, Windows support, licenses, and realistic latency on the target hardware.

## Data and privacy

- Microphone audio should be processed locally by default.
- Do not upload voice recordings or model inputs to a remote service.
- Store settings locally and explain where recordings are saved.
- Do not collect telemetry unless it is clearly disclosed and optional.

## Licensing

Review the license of every library, runtime, model, plugin, codec, and driver. A dependency being free to download does not automatically mean it may be bundled or redistributed. Keep a dependency/license inventory before packaging.
