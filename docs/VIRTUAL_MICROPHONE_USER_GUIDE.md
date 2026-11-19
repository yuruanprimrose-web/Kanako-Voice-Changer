# Kanako Virtual Microphone — User Setup Guide

Status: preparation guide for the first hardware prototype. The Kanako routing code and the VB-CABLE endpoint still require a successful Windows build and real-device validation.

## 1. Install the virtual audio cable

1. Open the official VB-Audio page: https://vb-audio.com/Cable/
2. Download the current Windows VB-CABLE package. As checked on 2026-11-19, the page lists **VBCABLE Driver Pack 46 (November 2026)**.
3. Extract the ZIP file to a normal folder.
4. Right-click the setup program and choose **Run as administrator**.
5. Approve the Windows driver installation prompt.
6. Restart Windows as instructed by VB-Audio.

Do not download the driver from third-party mirror sites. Do not disable Secure Boot or Windows driver-signing protections.

## 2. Confirm the Windows endpoints

After restart:

1. Open Windows Settings > System > Sound.
2. Open the list of all sound devices.
3. Confirm the playback/output endpoint **CABLE Input (VB-Audio Virtual Cable)** is present.
4. Confirm the recording/input endpoint **CABLE Output (VB-Audio Virtual Cable)** is present.

The naming is counterintuitive: Kanako sends audio to **CABLE Input** (a playback endpoint); communication apps receive it from **CABLE Output** (a recording endpoint).

## 3. Configure Kanako

Once a Windows build with the prototype is available:

1. Open Kanako.
2. Select your real microphone under **Microphone**.
3. Select your actual headphones/speakers under **Speaker / Output**.
4. Under **Virtual microphone**, select **CABLE Input**.
5. Keep your existing gain settings unchanged for the first test.
6. Start the voice changer and speak into the microphone.

Kanako should continue monitoring to your selected physical output while routing the processed voice to CABLE Input.

## 4. Select the microphone in another app

In Discord, Zoom, or OBS, open that application's audio/input settings and select **CABLE Output (VB-Audio Virtual Cable)** as the microphone/input device.

Do not select CABLE Input in the application's microphone list; it is the playback side of the virtual cable.

## 5. First test sequence

1. Start with Neutral / Bypass.
2. Speak and confirm you can hear yourself through your physical output.
3. Confirm the target app's input meter reacts to your voice.
4. Record a short test in the target app and listen to it.
5. Repeat with Female-style and Male-style presets.
6. Stop Kanako and note the virtual-microphone buffer diagnostics displayed by the app.
7. Only after the short test passes, run a longer 30-minute stability test.

If you hear echo or feedback, use headphones and lower the physical monitoring volume. Do not change the established gain defaults as a troubleshooting shortcut.

## 6. Uninstall

Use the uninstaller included in the official VB-CABLE package, run it as Administrator, and restart Windows as instructed by VB-Audio. Confirm that Kanako can still start with the virtual microphone option disabled.

## Important limitations

- This is a third-party driver, not a Kanako-owned driver.
- The driver is not bundled with Kanako in this prototype.
- Discord, Zoom, OBS, YouTube, and Facebook compatibility is not claimed until each workflow is tested.
- Driver install/uninstall requires administrator permission.
- Do not use test-signed drivers or weaken Windows security on a normal user PC.
