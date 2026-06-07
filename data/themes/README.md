# LEDSpicerUI — Theme Authoring Guide

This document describes the on-disk format every theme must follow. The
ThemeManager scans `share/ledspicerui/themes/` at startup; a theme is
discovered only when **both** `metadata.xml` **and** `preview.png` exist.
Anything else is silently skipped (with a stderr warning for missing
`preview.png`).

`*.md` files in this directory are excluded from the install.

---

## 1. Directory layout

```
data/themes/<id>/
├── metadata.xml            ← required
├── preview.png             ← required, 400×267, RGBA
├── dark/
│   ├── theme.css           ← required
│   └── 14 background PNGs  ← 1920×1080, RGBA
└── light/
    ├── theme.css           ← required
    └── 14 background PNGs  ← 1920×1080, RGBA
```

`<id>` is the directory name and the value persisted to user settings.
Use lowercase, no spaces (e.g. `default`, `arcadeNeon`, `modem`).

---

## 2. `metadata.xml`

```xml
<?xml version="1.0" encoding="UTF-8"?>
<theme name="Display Name" author="You" version="1.0.0">
    <description>One- or two-sentence vibe blurb. Shown as the tooltip on the theme selector tile.</description>
</theme>
```

Read by ThemeManager: `name` and `<description>`. `author` and `version`
are documentation only. There is **no** `preview=` attribute — the
preview file is mandatory at the path above.

---

## 3. Background images (14 per variant)

Filenames must match exactly. Both `dark/` and `light/` must provide all
14 files.

| File                          | Screen subject                                  |
|-------------------------------|-------------------------------------------------|
| `backgroundDevices.png`       | Microcontroller / chip on a circuit board       |
| `backgroundRestrictors.png`   | Joystick restrictor plate (octagon, 4-way, …)   |
| `backgroundGroups.png`        | Grouped nodes / clusters                        |
| `backgroundElements.png`      | Discrete LED elements                           |
| `backgroundProcesses.png`     | Signal flow / data pipeline diagram             |
| `backgroundColors.png`        | Color wheel / RGB spectrum                      |
| `backgroundInputs.png`        | Arcade buttons in a row                         |
| `backgroundInputSources.png`  | Cables / wires converging                       |
| `backgroundInputMaps.png`     | Key-to-action mapping grid                      |
| `backgroundInputLinkMaps.png` | Linked input chains                             |
| `backgroundAnimations.png`    | Waveforms / motion trails                       |
| `backgroundProfiles.png`      | Stacked cards / dossier folders                 |
| `backgroundLayout.png`        | Top-down arcade cabinet control panel layout    |
| `backgroundThemeSelector.png` | Stylized swatches / palette display             |

**Rules for the art**

- 1920×1080, PNG with alpha (artwork is rendered over the variant's
  base color; let transparent regions breathe).
- Low contrast and quiet — UI widgets sit on top.
- Dark variant: imagery on a near-black canvas, accents glow.
- Light variant: imagery on a near-white canvas, accents whisper.
- Same composition for the dark and light version of a given screen.
- Edges should fade or vignette so the image tiles into the GTK window
  without hard borders.

**`preview.png`** — 400×267, RGBA. Split mock: LEFT half shows the dark
variant UI, RIGHT half shows the light variant. One file represents
both variants on the selector.

---

## 4. `dark/theme.css` and `light/theme.css`

Each file has two sections:

1. A token table (`@define-color …`) — palette only.
2. Selectors that consume the tokens — copy verbatim from
   `default/<variant>/theme.css`, including the 14 `.zoneCanvas.<class>`
   `background-image` lines at the bottom.

The selectors block is the same for every theme. Only the palette
differs.

### Required `@define-color` tokens

| Group           | Tokens |
|-----------------|--------|
| State           | `color-toggle-checked`, `color-state-green-bg`, `color-state-red-bg`, `color-state-blue-bg` |
| BoxButton       | `color-box-bg`, `color-box-inner`, `color-box-border`, `color-box-text`, `color-box-btn-text` |
| Selection       | `color-selected-bg`, `color-selected-border`, `color-selected-glow` |
| Action hovers   | `color-edit-hover`, `color-copy-hover`, `color-delete-hover` |
| Type identity   | `color-type-device` (+`-glow`, +`-border`), `color-type-restrictor` (+`-glow`, +`-border`), `color-type-element` (+`-glow`, +`-border`), `color-type-group-system`, `color-type-directory` |
| Navigation      | `color-nav-hover` |
| Zone fallback   | `color-zone-bg` |
| Donation dialog | `color-support-text`, `color-support-hover`, `color-donate-title`, `color-donate-heart`, `color-donate-bg`, `color-donate-bg-hover`, `color-donate-text`, `color-donate-border`, `color-donate-glow` |

### Do **not** theme

The following are hardware-semantic and live in `style-base.css`. Do
not redefine them in `theme.css`:

- `.pinLabel`, `.pinSingle`, `.pinRed`, `.pinGreen`, `.pinBlue`, `.pinMulti`
- `.connectorBox`
- `.On`, `.Off`, `.Random`

The theme provider is loaded at a higher GTK priority than `style-base`
— overriding these classes from a theme will mask their colors across
the whole app.

---

## 5. CMake install

The themes directory is installed wholesale (with `*.md` excluded):

```cmake
install(DIRECTORY data/themes/ DESTINATION share/${PROJECT_NAME}/themes
    PATTERN "*.md" EXCLUDE)
```

A new theme dropped under `data/themes/<id>/` needs no CMake changes.
