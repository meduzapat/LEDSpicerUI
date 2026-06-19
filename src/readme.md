# LEDSpicerUI — Source & Developer Guide

Start here. This covers the source layout, how to build, the coding standards,
and where to dive deeper. Architecture details live in the per-layer guides
linked below — this page does not repeat them.

---

## Source Tree

| Path | Contents |
|---|---|
| `src/` | App entry (`LEDSpicerUI.cpp`), `Values`, `Defaults`, `XMLHelper`. |
| `src/config/` | File-backed config readers/writers (Settings, Project, Input, Animation, Profile, Geometry). |
| `src/Ui/` | Top-level window, theming, status bar, directory navigators, shared dialogs. |
| `src/Ui/Storage/` | The `Data` model and its collections — the core of the app. |
| `src/Ui/DataDialogs/` | Data-entry dialog controllers. |
| `src/Ui/Layout/` | Cabinet layout view and strip renderer. |
| `data/` | Glade UI, CSS, images — compiled into the binary as a GResource bundle. |
| `tests/` | GoogleTest suites and sample fixtures — see [tests/readme.md](../tests/readme.md). |

Everything under `data/` is embedded via GResource at build time; nothing there
is read from disk at runtime. Configuration and control XML *are* read at
runtime via tinyxml2.

---

## Build & Compile

Dependencies (Debian/Ubuntu package names):

```
build-essential cmake libgtkmm-3.0-dev libtinyxml2-dev libglib2.0-bin libxml2-utils
```

`libgtest-dev` is additionally required to build the tests. `libglib2.0-bin`
(`glib-compile-resources`) and `libxml2-utils` (`xmllint`) are build-only.

```sh
cmake -S . -B build/release                       # defaults to Release (-O3 -DNDEBUG)
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug # -g, no optimization
cmake --build build/release -j
```

`-DCMAKE_BUILD_TYPE` is the portable path — it works on every supported distro.
`CMakePresets.json` offers `debug`/`release` presets as a convenience but
requires CMake ≥ 3.21:

```sh
cmake --preset debug && cmake --build build/debug -j
```

Desktop integration is off by default. Pass `-DINSTALL_DESKTOP_ENTRY=ON` to
install a `.desktop` launcher and icon (desktop environments only).

Tests build only when GoogleTest is found — see [tests/readme.md](../tests/readme.md).

---

## Coding Standards

- tabs indent, spaces align after tabs; brace-initialization; readable operators (`and`, `or`, `not`).
- `PascalCase` types, `camelCase` members, `SCREAMING_CASE` constants; `is`/`has`/`can` boolean getters.
- `#pragma once`, never include guards; one class per file, named after the class.
- GTKmm3 only — no GTK4 APIs; libsigc++ for signals.
- raw pointers for owned non-widgets with a known lifetime; `Gtk::manage` / `Glib::RefPtr` for widgets; never delete what GTKmm owns.
- no defensive checks on states guaranteed by design — let invalid states crash loudly.
- new code must look like it belongs next to the code around it.

---

## Architecture

The data and dialog layers each have a dedicated guide. Read them in order;
start at the index:

- [Ui/readme.md](Ui/readme.md) — developer documentation index.
- [Ui/Storage/readme.md](Ui/Storage/readme.md) — the `Data` system (read first).
- [Ui/Storage/collections.md](Ui/Storage/collections.md) — collections and registries.
- [Ui/DataDialogs/readme.md](Ui/DataDialogs/readme.md) — dialog controllers.

---

## Contributing

- branch from `development`; keep each commit focused on one change.
- match the surrounding style — see the Coding Standards above.
- changing a class API → update its suite under `tests/` in the same commit ([tests/readme.md](../tests/readme.md)).
- build clean under `-Wall -Wextra` before pushing.
