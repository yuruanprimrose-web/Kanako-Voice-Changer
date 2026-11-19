# Kanako Virtual Microphone — Technical Investigation

Last reviewed: 2026-11-19

## Goal

Expose Kanako's processed voice as a Windows recording endpoint (a microphone device) so communication and streaming applications can select it as their input. Keep the DSP, pitch/formant controls, gain controls, and WAV recording inside the existing Kanako application. Do not create a separate client/demo app.

Target applications for later validation: Discord, Zoom, OBS Studio, YouTube live workflows, and Facebook live workflows. These are targets, not compatibility claims.

## Current baseline (verified)

- Build #52 is the current user-tested baseline.
- The user confirmed that WAV recordings contain audible voice.
- The app uses PortAudio for a full-duplex microphone-to-output stream and processes audio in the callback.
- The current stream has one selected physical output device. It does not yet expose a Windows virtual recording endpoint.
- Preserve the working presets, Pitch/Formant sliders, gain behavior, and recording pipeline while adding routing.

## Driver options investigated

### Option A — VB-Audio VB-CABLE as a first prototype bridge

VB-Audio's official licensing page states that VB-CABLE may be distributed with another application, including embedded/silent installation, provided its donationware model remains applicable. It also states that professional use in a company/institution/organization requires a paid license.

Advantages:
- Existing mature virtual playback/recording endpoints can provide a practical routing prototype.
- Avoids immediately authoring a new kernel-mode audio driver.
- The Kanako app can direct its processed output to the virtual cable's playback endpoint; other applications can select the corresponding recording endpoint.

Constraints:
- The package must be distributed AS IS under VB-Audio's terms; do not modify, rebrand, or imply ownership.
- Donationware/professional-use licensing must be resolved before shipping it with Kanako.
- Installation requires elevated driver installation privileges and a robust rollback/uninstall path.
- A simple output-device switch would route audio away from the user's speakers. Simultaneous virtual-mic and speaker monitoring needs an explicit output fan-out design and tests.
- The actual package version, signing status, installer behavior, and current terms must be rechecked before implementation/distribution.

Official licensing reference: https://vb-audio.com/Services/licensing.htm

### Option B — Build a Kanako-owned Windows virtual audio driver using Microsoft's SYSVAD sample as a reference

Microsoft documents SYSVAD as a WDM/WaveRT virtual audio driver sample and a starting point for custom audio drivers.

Advantages:
- Could ultimately expose a Kanako-branded recording endpoint without depending on a third-party cable product.
- Gives Kanako control over endpoint naming and long-term routing behavior.

Constraints:
- SYSVAD is a driver-development sample, not a ready-made production Kanako virtual microphone. Microsoft notes that its sample simulates capture/loopback with a tone and does not implement real audio mixing.
- It requires Visual Studio, Windows SDK, WDK, driver-specific build/test setup, and target-machine testing.
- Kernel-mode virtual drivers have the same signing requirements as hardware drivers. Production distribution requires Microsoft dashboard signing; current Microsoft guidance describes EV certificate requirements for dashboard submissions and recommends HLK-tested WHCP signing for production.
- Test-signing is for development only and is not an acceptable customer installation workaround. Never ask users to disable Secure Boot or weaken Windows driver security to run a production Kanako build.
- This route has materially higher engineering, certification, and support cost.

Official references:
- SYSVAD sample: https://learn.microsoft.com/en-us/samples/microsoft/windows-driver-samples/sysvad-virtual-audio-device-driver-sample/
- Virtual audio devices: https://learn.microsoft.com/en-us/windows-hardware/drivers/audio/virtual-audio-devices
- Kernel-mode signing: https://learn.microsoft.com/en-us/windows-hardware/drivers/install/kernel-mode-code-signing-requirements--windows-vista-and-later-

## Recommendation

Do not write or ship a custom kernel driver yet. First prototype the routing architecture against a legally usable, already signed virtual cable package, while keeping the adapter replaceable so a future Kanako-owned driver can be substituted.

Before bundling VB-CABLE in a public Kanako installer, explicitly resolve its donationware/professional-use terms and redistribution conditions. Do not silently purchase licenses or claim it is free for every commercial deployment.

If its terms or product requirements do not fit, stop the VB-CABLE route and scope a custom driver project separately, including signing/certification costs and a real Windows target-machine test plan.

## Required engineering work before any compatibility claim

1. Inspect the exact virtual cable package and confirm architecture, supported Windows versions, endpoint names, signatures, and installer/uninstaller commands.
2. Design a routing layer that sends the processed mono signal to the virtual microphone endpoint and, optionally, to the user's selected speakers at the same time.
3. Avoid blocking, allocations, Qt calls, or file/network operations in real-time audio callbacks. Use bounded, thread-safe buffering between the processing callback and any additional output stream.
4. Handle device disappearance, app shutdown, driver removal, sample-rate mismatch, and failed stream startup without crashing or leaving devices stuck.
5. Add automated tests for routing/buffer behavior and manual tests on a real Windows machine.
6. Verify that Discord, Zoom, and OBS can select the endpoint and receive intelligible processed speech. Test YouTube/Facebook only through their actual supported streaming workflows; they may consume OBS or browser audio inputs differently.
7. Test install, upgrade, repair, rollback, and uninstall with administrator privileges and a clean Windows environment.

## Safety and release gates

- Never install an unsigned or test-signed driver on an ordinary user's PC.
- Never disable Secure Boot or Windows driver-signing enforcement as a release workaround.
- Do not bundle third-party driver files until the exact redistribution terms and required notices are recorded.
- Do not change the existing pitch/formant ranges, preset values, microphone/output gain behavior, or recording implementation as part of this work unless separately authorized.
- A successful CI build is not proof of virtual-microphone compatibility. Only real Windows endpoint and application tests can establish that.
