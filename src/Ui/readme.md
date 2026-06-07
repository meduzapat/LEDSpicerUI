# LEDSpicerUI — Developer Documentation Index

This index links the three subsystem guides. Read them in order if you are new
to the codebase; jump to a specific one if you need a refresher on a particular
layer.

---

## [Storage/readme.md](Storage/readme.md) — Data System

The foundation. Covers the `Data` class and its entire hierarchy:

- `Values` field store — inherited `values` (serialized) vs the `properties` member (runtime-only)
- `toXML()`, `xmlBody()`, `createUniqueId()`, `createPrettyName()`, `getCssClass()`
- Composition — `Parent` subclasses owning child `BoxButtonCollection`s, plus `Parent::registerDependency`
- `Link` and `InputMap` — reference wrappers pointing to another `Data`
- `DirNode` mixin, `DirectoryEntry`, and the file-backed types (`Input`, `Animation`, `Profile`)
- `setUp()` / `tearDown()` lifecycle hooks
- `shouldSerialize()` — opt-out for fields you don't want emitted

**Read this first.** Everything else builds on these concepts.

---

## [Storage/collections.md](Storage/collections.md) — Collections System

The two collection types and how they relate:

- `BoxButtonCollection` — ordered, UI-linked, owns its `BoxButton`s and `Data`
- `CollectionHandler` — global named registry, keyed lookup, cascade deletes
- Global vs scoped collection names
- `add` / `remove` / `replace` semantics
- `Parent::registerDependency` — preferred cascade wiring for child families;
  `CollectionHandler::registerDependency` for dialog-local collections
- `refreshComboBox(combo)` / `refreshComboBox(combo, excludeProperties)` — explicit combo repopulation
- `SensitivityTracker` — automatic widget enable/disable based on collection size
- `release()` pairing rules and the `purgeAll()` teardown order

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
- `DialogFormHost` — type-selector dialogs with `onConvert` / `onEmpty` / `onSelected`
- `DirectoryAware` — directory-scoped file dialogs (`DialogInput`, `DialogAnimation`, `DialogProfile`)
- `DialogSelect` and `SelectionRequest` — picking from existing items
- Step-by-step guide for creating a new dialog

**Read after the Collections guide.** Dialogs are the only layer that should
call into both `BoxButtonCollection` and `CollectionHandler` directly.

---

## Layer Dependency Summary

```
DialogForm  (DataDialogs/)
    │  reads / writes via storeData() / retrieveData()
    ▼
Data subclasses  (Storage/)               ◄── registered (raw pointer) in ──┐
    ▲                                                                       │
    │  owned by                                                             │
    │                                                                CollectionHandler
BoxButton  (Storage/)                                                    (Storage/)
    ▲                                                                       │
    │  held in                                                              │ cascade
    │                                                                       │ deletes /
BoxButtonCollection  (Storage/)  ◄────── registered as dependency of ───────┘ comboboxes
```

- `BoxButtonCollection` is the sole owner of `BoxButton`, which is the sole
  owner of its `Data*`.
- `CollectionHandler` holds only raw `Data*` pointers for global lookup and
  cascade delete; it never deletes the `Data` itself.
- A change in `Data` or `BoxButtonCollection` will ripple upward into
  `CollectionHandler` and then into the dialog layer. A change confined to
  a `DialogForm` subclass affects only that dialog.