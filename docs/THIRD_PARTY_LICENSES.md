# Third-Party Dependency and License Inventory

This is a planning inventory. A listed component is a candidate, not proof that it has already been integrated or shipped. Before release, record the exact version/commit, license file, changes made, and files included in the installer.

| Component | Intended use | Upstream license | Current status |
|---|---|---|---|
| Qt 6 Widgets | Integrated Windows desktop UI | LGPLv3/GPLv3 or commercial, depending on distribution terms | Candidate; use LGPL route only after compliance review and dynamic-linking plan |
| PortAudio | Microphone capture and speaker playback | MIT | Candidate; not yet integrated |
| Signalsmith Stretch | Pitch/time shifting and formant compensation | MIT | Candidate; not yet integrated |

## Required release checks

- Include each dependency's original copyright and license text.
- Pin exact versions or commit hashes; do not build releases from floating branches.
- Review Qt module-specific licensing and LGPL obligations for the actual packaging method.
- Confirm whether any transitive dependency has a different license.
- Do not bundle a virtual audio driver, codec, model, or other binary until its redistribution terms are checked.
- Generate a complete third-party notices file for every installer release.

## Upstream references

- Qt for Python licensing overview: https://pypi.org/project/PySide6/ (the project page describes PySide6 licensing; the final C++ Qt modules must be checked against their own official notices).
- PortAudio license: https://portaudio.com/license.html
- Signalsmith Stretch source and MIT license: https://github.com/Signalsmith-Audio/signalsmith-stretch

Last reviewed: 2026-11-19.
