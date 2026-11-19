# Testing Strategy

No test results are claimed by this document. Record actual environment details, commands, and outputs when tests are run.

## Development checks

- Static checks and formatting for each selected language.
- Unit tests for settings, model metadata validation, and audio lifecycle state.
- Integration tests for microphone capture and output playback.
- Windows packaging test on a clean machine/VM.

## Manual acceptance checklist

### Audio devices
- [ ] Microphone dropdown lists available input devices.
- [ ] Speaker/output dropdown lists available output devices.
- [ ] Input level meter responds to speech.
- [ ] Device unplug/replug is handled without crashing.

### Virtual microphone prototype (VB-CABLE)

- [ ] Install the official VB-CABLE package using its administrator setup program; reboot as instructed by VB-Audio.
- [ ] Confirm PortAudio lists "CABLE Input" as a playback device and Kanako lists it in the optional Virtual microphone selector.
- [ ] Start Kanako with a physical microphone, physical speaker/headphones, and CABLE Input selected as the virtual microphone.
- [ ] Confirm the physical speaker/headphones still receive the processed voice.
- [ ] Confirm the Windows recording endpoint "CABLE Output" receives the same processed voice.
- [ ] Confirm Discord can select CABLE Output and receive intelligible processed speech.
- [ ] Repeat with Zoom and OBS; test YouTube/Facebook workflows separately.
- [ ] Run a 30-minute test and check for dropouts, silence, buffer overflow/underflow, and device recovery.
- [ ] Stop Kanako and verify both output streams close cleanly.
- [ ] Uninstall VB-CABLE using its official uninstaller and reboot as instructed; confirm Kanako falls back to physical output without crashing.

These are pending manual tests. A successful CI build or synthetic buffer test does not prove that a real Windows virtual microphone works.
- [ ] Start Recording begins capture.
- [ ] Stop Recording ends capture and finalizes a valid WAV.
- [ ] Play Recording plays the saved WAV.
- [ ] Invalid or unavailable devices produce a useful error.

### Voice presets
- [ ] Female presets can be selected and applied.
- [ ] Male presets can be selected and applied.
- [ ] Processed speech remains intelligible.
- [ ] Start/stop and preset switching do not leave audio devices locked.

### Installer
- [ ] Installation progress reflects real work.
- [ ] Normal installation does not expose an unnecessary console window.
- [ ] App launches after installation.
- [ ] Upgrade/repair/uninstall behavior is verified.

## Test report template

- Date:
- Windows version:
- Hardware/CPU:
- GPU (if applicable):
- App commit:
- Build command and result:
- Test performed:
- Expected result:
- Actual result:
- Logs or screenshots:
- Pass/fail:
