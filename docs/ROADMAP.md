# Kanako Voice Changer — Roadmap

This roadmap distinguishes repository setup from implemented and tested functionality. Items remain pending until there is code and evidence from builds/tests.

## Phase 0 — Repository foundation

- [x] Create public GitHub repository.
- [x] Add project overview and initial documentation.
- [x] Define a proposed repository layout.
- [ ] Select and add an explicit open-source license.
- [ ] Confirm desktop framework and audio/voice-engine architecture.
- [ ] Add contribution and security-reporting guidance.

## Phase 1 — Desktop application foundation

- [ ] Create the integrated Kanako Voice Changer desktop application.
- [ ] Establish application identity, window behavior, and settings storage.
- [ ] Add microphone and speaker device enumeration.
- [ ] Add audio-level meter with reliable device refresh.
- [ ] Add clear error messages for missing devices and engine failures.

Acceptance: application launches on a clean supported Windows installation; devices are listed correctly; the UI remains responsive.

## Phase 2 — Audio pipeline

- [ ] Implement microphone capture and output playback.
- [ ] Implement start/stop lifecycle and safe device switching.
- [ ] Add recording and WAV playback for diagnostics.
- [ ] Test latency, audio dropouts, clipping, and device disconnection.

Acceptance: microphone input can be monitored/recorded and played back reliably without voice conversion enabled.

## Phase 3 — Voice presets and conversion

- [ ] Select a locally runnable voice-conversion engine.
- [ ] Verify the engine's license and redistribution terms.
- [ ] Define female and male preset categories.
- [ ] Add model download/import and integrity checks.
- [ ] Connect preset selection to the live audio pipeline.
- [ ] Test intelligibility, latency, CPU/GPU compatibility, and failure recovery.

Acceptance: each shipped or downloadable preset is legally distributable or clearly documented as user-supplied; speech remains intelligible in supported configurations.

## Phase 4 — Packaging and installation

- [ ] Package all required runtime components into the Kanako installation flow.
- [ ] Avoid exposing unnecessary command windows during normal installation/use.
- [ ] Add a real progress indicator tied to actual installation stages.
- [ ] Add upgrade, repair, and uninstall behavior.
- [ ] Test on a clean Windows machine or clean VM.

Acceptance: a user can install, launch, update, and uninstall Kanako without manually assembling separate demo applications.

## Phase 5 — Quality and release

- [ ] Add automated tests and a Windows build workflow.
- [ ] Run security and dependency-license checks.
- [ ] Test microphone/speaker selection and common device changes.
- [ ] Test female and male presets on supported hardware.
- [ ] Publish a release only after the installer and test results are verified.

## Scope guardrails

- No paid API requirement.
- No subscription or premium feature lock.
- No separate Voice Changer Client Demo application.
- No proprietary VoiceWave code, assets, or binaries copied into Kanako.
- Do not claim compatibility with hardware or operating systems that have not been tested.
