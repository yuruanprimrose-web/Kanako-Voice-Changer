# Kanako Voice Changer — Roadmap

This roadmap separates repository/documentation work from implemented and tested functionality. An item is not complete until source code exists and the relevant build/test evidence is recorded.

## Phase 0 — Repository foundation

- [x] Create public GitHub repository.
- [x] Add project overview and initial documentation.
- [x] Define repository layout.
- [x] Select and add the MIT license for Kanako's own source.
- [x] Record the non-AI DSP direction and candidate technology stack.
- [ ] Add contribution and security-reporting guidance.
- [ ] Pin and audit exact third-party dependency versions and notices.

## Phase 1 — Integrated desktop application foundation

- [ ] Create the Qt 6 Widgets Windows application.
- [ ] Establish Kanako application identity, window behavior, and local settings storage.
- [ ] Add microphone and speaker device enumeration.
- [ ] Add an input-level meter with reliable device refresh.
- [ ] Add clear errors for missing devices and audio initialization failures.

Acceptance: the integrated Kanako window launches on a supported Windows installation, lists devices correctly, and remains responsive. No separate client/demo window is launched.

## Phase 2 — Audio pipeline

- [ ] Implement microphone capture and speaker playback through PortAudio.
- [ ] Implement start/stop lifecycle and safe device switching.
- [ ] Add neutral/bypass monitoring.
- [ ] Add recording and WAV playback for diagnostics (only on explicit user action).
- [ ] Test latency, audio dropouts, clipping, and device disconnection.

Acceptance: clean microphone audio can pass through the app reliably with conversion disabled.

## Phase 3 — Built-in voice presets (non-AI DSP)

- [ ] Integrate and compile the pitch/formant processing library.
- [ ] Implement the DSP processing adapter with preallocated callback buffers.
- [ ] Add female-style, male-style, and neutral/bypass presets.
- [ ] Tune pitch and formant parameters using speech listening tests.
- [ ] Verify intelligibility, latency, CPU usage, and failure recovery.

Acceptance: each preset works in the live microphone-to-output path, speech remains understandable, and the result is not just a UI selection or an unconnected demo.

## Phase 4 — Packaging and installation

- [ ] Select installer technology after the app builds reliably.
- [ ] Package the integrated app and required runtime components.
- [ ] Avoid unnecessary visible command windows during normal use.
- [ ] Add a real progress indicator tied to actual installation stages.
- [ ] Add upgrade, repair, and uninstall behavior.
- [ ] Include all required third-party license notices.
- [ ] Test on a clean Windows machine or clean VM.

Acceptance: a user can install, launch, update, and uninstall Kanako without manually assembling separate applications.

## Phase 5 — Quality and release

- [ ] Add automated tests and a Windows build workflow.
- [ ] Run dependency/license and security checks.
- [ ] Test device selection and common device changes.
- [ ] Test all presets on supported hardware.
- [ ] Publish a release only after the installer and test results are verified.

## Phase 6 — Virtual microphone integration (next priority)

- [x] Research Microsoft's SYSVAD virtual audio driver sample and Windows virtual-device model.
- [x] Review VB-Audio VB-CABLE redistribution and professional-use licensing conditions.
- [x] Record the driver options, routing risks, and production release gates in [Virtual Microphone Plan](VIRTUAL_MICROPHONE_PLAN.md).
- [ ] Verify a legally usable, correctly signed virtual-cable package and exact redistribution terms before bundling anything.
- [ ] Implement processed-audio routing to a Windows virtual recording endpoint.
- [ ] Design simultaneous virtual-microphone output and physical-speaker monitoring without changing the established gain behavior.
- [ ] Add device-loss, startup/shutdown, buffering, and recovery handling.
- [ ] Test endpoint installation/removal on a real Windows machine.
- [ ] Verify processed speech is selectable and intelligible in Discord, Zoom, and OBS.
- [ ] Verify YouTube and Facebook live workflows separately before claiming compatibility.
- [ ] Complete upgrade, repair, rollback, and uninstall tests for the driver package.

Acceptance: a real Windows recording endpoint receives Kanako's processed speech reliably, can be selected by the target applications, and can be installed/removed without weakening Windows security. A successful CI build alone does not satisfy acceptance.

## Scope guardrails

- No AI engine, model training, or AI model download.
- No paid API requirement, subscription, or premium feature lock.
- No separate Voice Changer Client Demo application.
- No proprietary VoiceWave code, assets, or binaries copied into Kanako.
- Do not claim compatibility with hardware or operating systems that have not been tested.
- Do not claim a feature is complete based only on a mock UI or placeholder implementation.
