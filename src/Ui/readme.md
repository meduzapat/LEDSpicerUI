# LEDSpicerUI — Developer Documentation Index

> **Status:** Work in progress — reflects design as of v0.0.7 / data format 1.1.

This index links the three subsystem guides. Read them in order if you are new
to the codebase; jump to a specific one if you need a refresher on a particular
layer.

---

## [Storage/readme.md](Storage/readme.md) — Data System

The foundation. Covers the `Data` class and its entire hierarchy:

- `fieldsData` (serialized) vs `properties` (runtime-only)
- `toXML()`, `createUniqueId()`, `createPrettyName()`, `getCssClass()`
- Composed data — `Data` that owns child `BoxButtonCollection`s
- `Link` — reference wrapper pointing to another `Data`
- `DirNode`, `DirectoryEntry`, `FileData` — the file tree types
- `activate()` / `deActivate()` lifecycle hooks
- The `ignored` field set (provisional)

**Read this first.** Everything else builds on these concepts.

---

## [Storage/collections.md](Storage/collections.md) — Collections System

The two collection types and how they relate:

- `BoxButtonCollection` — ordered, UI-linked, owns its `BoxButton`s and `Data`
- `CollectionHandler` — global named registry, keyed lookup, cascade deletes
- Global vs scoped collection names
- `add` / `remove` / `replace` semantics
- `Dependency` — cascade delete registration with optional depletion callbacks
- `ComboBoxText` subscribers — auto-refresh on add/remove
- `release()` — pairing rule for every `registerDependency` call
- Teardown order and `purgeAll()`

**Read after the Data guide.** Required before touching anything that crosses
dialog boundaries.

---

## [DataDialogs/readme.md](DataDialogs/readme.md) — Dialog System

The UI controller layer. Covers `DialogForm` and all its specializations:

- `SingletonDialog<T>` / `GladeDialog<T>` — singleton pattern and builder wiring
- Stale (`currentData`, `ownerData`) vs fresh (widgets) data
- The three form flows — ADD, EDIT, LOAD — step by step
- Primary vs secondary dialogs and `setOwner()`
- `BoxButton` decoration — edit, delete, clone buttons
- Child dialog instantiation from builder and the refresh chain
- The Add button — lives in the parent layout, wired by the child dialog
- `DialogFileForm` — directory-scoped file dialogs
- `DialogSelect` and `SettingRequest` — picking from existing items
- Step-by-step guide for creating a new dialog

**Read after the Collections guide.** Dialogs are the only layer that should
call into both `BoxButtonCollection` and `CollectionHandler` directly.

---

## Layer Dependency Summary

```
DialogForm  (DataDialogs/)
    │  reads/writes
    ▼
Data subclasses  (Storage/)
    │  registered in
    ▼
CollectionHandler  (Storage/)
    │  owns BoxButtons via
    ▼
BoxButtonCollection  (Storage/)
    │  wraps
    ▼
BoxButton → Data*
```

A change in `Data` or `BoxButtonCollection` will ripple upward into
`CollectionHandler` and then into the dialog layer. A change confined to
a `DialogForm` subclass affects only that dialog.