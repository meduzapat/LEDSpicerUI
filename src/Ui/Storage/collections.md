# LEDSpicerUI — Collections System Developer Guide

---

## Table of Contents

1. [Overview](#1-overview)
2. [BoxButtonCollection](#2-boxbuttoncollection)
3. [CollectionHandler](#3-collectionhandler)
4. [SensitivityTracker](#4-sensitivitytracker)
5. [Global vs Scoped Collections](#5-global-vs-scoped-collections)
6. [Adding, Removing, and Replacing Items](#6-adding-removing-and-replacing-items)
7. [Dependencies — Cascade Deletes](#7-dependencies--cascade-deletes)
8. [ComboBox Subscribers](#8-combobox-subscribers)
9. [Widget Sensitivity Subscribers](#9-widget-sensitivity-subscribers)
10. [Releasing Registrations](#10-releasing-registrations)
11. [Querying the Registry](#11-querying-the-registry)
12. [Lifecycle and purgeAll](#12-lifecycle-and-purgeall)
13. [Quick Reference](#13-quick-reference)

---

## 1. Overview

| | `BoxButtonCollection` | `CollectionHandler` |
|---|---|---|
| **Lives** | Inside a `Data` object or a dialog | Global static registry |
| **Purpose** | Ordered, UI-linked list of items | Keyed lookup and cascade management |
| **Keyed by** | Insertion order | `Data::createUniqueId()` string |
| **Owns `Data`?** | Yes — via `BoxButton` | No — raw pointer only |
| **Mixin** | `SensitivityTracker` | `SensitivityTracker` |

A `Data` item typically appears in **both**: its `BoxButton` lives in a
`BoxButtonCollection`, and a raw pointer is registered in a `CollectionHandler`
for cross-dialog lookups and cascade deletes.

---

## 2. BoxButtonCollection

Ordered `vector<BoxButton*>`. Owns every `BoxButton` it holds, which in turn
owns the `Data*`. Inherits `SensitivityTracker`.

```cpp
BoxButton& bb = collection.create(myData); // takes ownership; fires sensitivity refresh
collection.remove(bb);                      // deletes BoxButton + Data; fires sensitivity refresh
collection.remove(myData);                  // same, by Data pointer
collection.wipe();                          // deletes all, clears vector; fires sensitivity refresh
collection.populateBox(box);                // fills GTK flow box
collection.reindex(box);                    // re-syncs order to visual order
collection.swap(other);                     // O(1) contents exchange
```

`create(Data*)` is the **only** correct way to hand a `Data*` to a collection.

```cpp
collection.isSet(myData);      // checks by createUniqueId()
collection.isIdSet("someId");  // same, by string
```

---

## 3. CollectionHandler

Named singleton factory. Each name maps to one independent instance. Inherits
`SensitivityTracker`.

```cpp
CollectionHandler* ch = CollectionHandler::getInstance(COLLECTION_ELEMENT);
ch->add(myData);
ch->isSet(myData);
Data* found = ch->get("myId");  // nullptr if not found — always check
```

`add()` is idempotent — silently skips items already registered.
`remove()` guards against empty or unknown IDs — safe to call unconditionally.

---

## 4. SensitivityTracker

Mixin inherited by both `BoxButtonCollection` and `CollectionHandler`. Keeps
registered `Gtk::Widget` sensitivity in sync with the collection size
automatically — no manual `set_sensitive` calls needed at call sites.

`refreshSensitiveWidgets()` is called internally on every `add()`, `remove()`,
`create()`, and `wipe()`. Registration fires an immediate evaluation.

```cpp
// Register — widget is updated immediately and on every future change.
ch->registerSensitivity(btnAdd);           // sensitive when size >= 1
ch->registerSensitivity(btnApply, 2);      // sensitive only when size >= 2

// Unregister — only needed when the widget can outlive the collection
// in a scoped/temporary context. Singletons and program-lifetime objects
// do not need to release.
ch->releaseSensitive(btnAdd);
```

---

## 5. Global vs Scoped Collections

**Global** — meaningful across the entire project:

```cpp
COLLECTION_ELEMENT, COLLECTION_GROUP, COLLECTION_ANIMATIONS, COLLECTION_INPUT
```

**Scoped** — valid only within one parent, name = base constant + parent ID:

```cpp
string id = COLLECTION_INPUT_SOURCES + inputData->getProperties().getValue(UID);
CollectionHandler::getInstance(id)->add(mySource);
```

Scoped collections are created lazily on first `getInstance()` and persist
until `purgeAll()`.

> Global: item reachable from multiple places. Scoped: item only makes sense within one parent.

---

## 6. Adding, Removing, and Replacing Items

```cpp
ch->add(myData);             // indexes by createUniqueId(); refreshes sensitivity; no-op if already present
ch->remove(myData);          // removes; cascades; refreshes sensitivity; no-op if not present
ch->replace(myData, oldId);  // re-keys if ID changed; no cascade; no sensitivity refresh
```

`replace` is a no-op when the new ID equals the old one.

---

## 7. Dependencies — Cascade Deletes

A dependency wires a `BoxButtonCollection` to a watched
`CollectionHandler` so that when an item is removed from the global
registry, every entry referencing it is removed from the dependent
collection too.

### Preferred API — `Parent::registerDependency`

When the dependent collection is a child of a `Parent` (the usual case),
register the dependency from the `Parent` constructor using the family
name pair:

```cpp
// Group.cpp — when a global Element is removed, drop any Link to it
// from this Group's link-collection.
registerDependency(COLLECTION_ELEMENTS, COLLECTION_GROUP_LINKS);

// Profile.cpp — every profile family follows its global counterpart.
registerDependency(COLLECTION_ELEMENTS,   COLLECTION_PROFILE_ELEMENTS);
registerDependency(COLLECTION_GROUPS,     COLLECTION_PROFILE_GROUPS);
registerDependency(COLLECTION_INPUTS,     COLLECTION_PROFILE_INPUTS);
registerDependency(COLLECTION_ANIMATIONS, COLLECTION_PROFILE_ANIMATIONS);
```

`Parent` looks up the watched `CollectionHandler*` and the matching child
`BoxButtonCollection*` and forwards the pairing to the handler. The
`Parent` destructor releases every dependency it registered, so callers
do not have to track the pairings themselves.

### Low-level API — `CollectionHandler::registerDependency`

For collections that are not owned by a `Parent` (typically dialog-local
collections) the dependency is registered directly on the handler:

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENTS)->registerDependency(&maps);
```

The signature is a single pointer — there is no min-size guard or
depletion callback. Pair every direct call with
`release(BoxButtonCollection*)` in the owner's destructor (see
[§10](#10-releasing-registrations)).

---

## 8. ComboBox Refresh

ComboBoxes are not auto-subscribed any more — there is no implicit
re-population on `add()` / `remove()`. Refresh the combo explicitly when
it becomes visible (dialog open, stack page switch, selector reopened):

```cpp
// Simple refresh — one row per item in the collection.
CollectionHandler::getInstance(COLLECTION_ELEMENTS)->refreshComboBox(myCombo);

// Filter out items whose properties match any of the given keys
// (used by selectors that hide already-consumed items).
CollectionHandler::getInstance(COLLECTION_ELEMENTS)
    ->refreshComboBox(myCombo, {ASSIGNED_TO_GROUP});
```

Because the combo is not registered with the handler there is nothing to
release; the combo can be destroyed at any time without notifying the
handler.

---

## 9. Widget Sensitivity Subscribers

See [SensitivityTracker](#4-sensitivitytracker) for the full API.

Registration sites are constructors of navigators and dialogs. Since all
current consumers are singletons that live for the program lifetime alongside
their collections, no `releaseSensitive` call is needed.

```cpp
// InputDirectoryNavigator constructor:
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerSensitivity(btnAddInput);
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerSensitivity(btnImportInput);

// DialogProfile constructor:
CollectionHandler::getInstance(COLLECTION_ELEMENT   )->registerSensitivity(btnProfilesAddElements);
CollectionHandler::getInstance(COLLECTION_GROUP      )->registerSensitivity(btnProfilesAddGroups);
CollectionHandler::getInstance(COLLECTION_ANIMATIONS )->registerSensitivity(btnProfilesAddAnimations);
CollectionHandler::getInstance(COLLECTION_INPUT      )->registerSensitivity(btnProfilesAddInputs);
```

`BoxButtonCollection` supports the same API for local child collections:

```cpp
// DialogDevice — btnApply sensitive only when at least one element exists:
elements.registerSensitivity(btnApply);
```

---

## 10. Releasing Registrations

| Registration | Needs paired release? |
|---|---|
| `Parent::registerDependency` | No — released automatically in the `Parent` destructor |
| `CollectionHandler::registerDependency` | Yes — in destructor of the owner |
| `registerSensitivity` | Only if the widget can be destroyed before the collection |

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENTS)->release(&myCollection);
CollectionHandler::getInstance(COLLECTION_ELEMENTS)->releaseSensitive(myWidget);
```

---

## 11. Querying the Registry

```cpp
Data*  found = ch->get("myId");                       // nullptr if not found
bool   has   = ch->isSet(myData);                     // by createUniqueId()
bool   hasId = ch->isIdSet("someId");
size_t n     = ch->countByKey(TYPE, "1");
vector<Data*> v = ch->findByProperty(PID, parentId);
bool   any   = ch->hasAny(PID, parentId);             // existence-only — cheaper than findByProperty
```

---

## 12. Lifecycle and purgeAll

`CollectionHandler` does not own `Data*` pointers. Correct teardown order:

1. Destroy all `BoxButtonCollection`s — recursively destroys all `Data`, which
   call `unregisterFromCollection()` during destruction.
2. Call `CollectionHandler::purgeAll()` to clean up the now-empty instances.

Reversing this order leaves dangling pointer calls in `Data` destructors.

---

## 13. Quick Reference

### BoxButtonCollection

| Method | Effect |
|--------|--------|
| `create(Data*)` | Takes ownership; returns `BoxButton&`; fires sensitivity refresh. |
| `remove(BoxButton&)` | Deletes `BoxButton` and `Data`; fires sensitivity refresh. |
| `remove(Data*)` | Same, by Data pointer. |
| `wipe()` | Deletes all, clears vector; fires sensitivity refresh. |
| `populateBox(box)` | Adds all buttons to a GTK flow box. |
| `reindex(box)` | Re-syncs order to visual order. |
| `swap(other)` | O(1) contents exchange; dependency addresses stay valid. |
| `getSize()` | Number of items. |
| `isSet(Data*)` | Checks by `createUniqueId()`. |
| `isIdSet(string)` | Checks by id string. |
| `registerSensitivity(widget, n)` | Widget sensitive when size >= n (default 1). |
| `releaseSensitive(widget)` | Removes sensitivity binding. |

### CollectionHandler

| Method | Effect |
|--------|--------|
| `getInstance(name)` | Returns (or creates) the named instance. |
| `removeInstance(name)` | Destroys a single named instance. |
| `add(Data*)` | Indexes by `createUniqueId()`; refreshes sensitivity; no-op if present. |
| `remove(Data*)` | Removes; cascades to dependent collections; refreshes sensitivity; no-op if absent. |
| `replace(Data*, oldId)` | Re-keys if id changed; no cascade. |
| `get(id)` | Returns `Data*` or `nullptr`. |
| `isSet(Data*)` | Existence check by `createUniqueId()`. |
| `isIdSet(string)` | Existence check by id string. |
| `countByKey(key, value)` | Count items where `getValue(key) == value`. |
| `findByProperty(prop, value)` | Returns all items with matching property. |
| `hasAny(prop, value)` | Existence-only variant of `findByProperty` — cheaper when only the boolean matters. |
| `registerDependency(BoxButtonCollection*)` | Registers a cascade target. Use `Parent::registerDependency` when possible. |
| `release(BoxButtonCollection*)` | Removes a dependency registration. |
| `refreshComboBox(combo)` | Repopulates one combo from the collection. |
| `refreshComboBox(combo, excludeProperties)` | Same, but skips items whose properties match any of the given keys. |
| `registerSensitivity(widget, n)` | Widget sensitive when size >= n (default 1). |
| `releaseSensitive(widget)` | Removes sensitivity binding. |
| `purgeAll()` | Destroys all named instances. |
