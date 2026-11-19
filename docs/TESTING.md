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

### Recording diagnostics
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
