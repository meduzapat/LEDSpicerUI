# LEDSpicerUI — Tests

GoogleTest suites for the non-UI layers: the `Data` model, collections, config
file readers/writers, and helpers. Part of the developer docs — see
[../src/readme.md](../src/readme.md) for the source layout and build basics.

---

## Building & Running

Tests build only when GoogleTest (≥ 1.8) is found at configure time, and are
attached to the main build via `add_subdirectory(tests)`.

```sh
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j
cd build/debug/tests && ctest --output-on-failure
```

Some suites instantiate GTK widgets and need a display. Headless/CI runs wrap
`ctest` in Xvfb:

```sh
xvfb-run --auto-servernum ctest --output-on-failure
```

This mirrors what the CI workflow and `Dockerfile` do.

---

## Layout

| Path | Contents |
|---|---|
| `tests/Ui/Storage/` | `Data` model and collection suites. |
| `tests/config/` | Config file reader/writer suites. |
| `tests/mocks/` | Lightweight `Data` doubles (`MockData`, `MockBasicData`, `ElementObserverMock`). |
| `tests/data/` | Sample XML fixtures, reached at runtime via `PACKAGE_SAMPLES_DIR`. |
| `tests/CMakeLists.txt` | Suite registration and shared source groups. |

---

## Adding a Test

- one suite per class, named `<Class>Test.cpp`.
- register it in `tests/CMakeLists.txt` with `add_test_executable`, reusing the
  existing source groups (`STORAGE_SRCS`, `STORAGE_BASE_SRCS`, …) instead of
  relisting sources.
- changing a class API → update its suite in the same commit.
