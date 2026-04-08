# LEDSpicerUI — Collections System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.13 / data format 1.1.

---

## Table of Contents

1. [Overview — Two Collection Types](#1-overview--two-collection-types)
2. [BoxButtonCollection](#2-boxbuttoncollection)
3. [CollectionHandler](#3-collectionhandler)
4. [Global vs Scoped Collections](#4-global-vs-scoped-collections)
5. [Adding, Removing, and Replacing Items](#5-adding-removing-and-replacing-items)
6. [Dependencies — Cascade Deletes](#6-dependencies--cascade-deletes)
7. [ComboBox Subscribers](#7-combobox-subscribers)
8. [Releasing Registrations](#8-releasing-registrations)
9. [Querying the Registry](#9-querying-the-registry)
10. [Lifecycle and purgeAll](#10-lifecycle-and-purgeall)
11. [Quick Reference](#11-quick-reference)

---

## 1. Overview — Two Collection Types

| | `BoxButtonCollection` | `CollectionHandler` |
|---|---|---|
| **Lives** | Inside a `Data` object or a dialog | Global static registry |
| **Purpose** | Ordered, UI-linked list of items | Keyed lookup and cascade management |
| **Keyed by** | Insertion order | `Data::createUniqueId()` string |
| **Owns `Data`?** | Yes — via `BoxButton` | No — raw pointer only |

A `Data` item typically appears in **both**: its `BoxButton` lives in a `BoxButtonCollection`, and a raw pointer is registered in a `CollectionHandler` for cross-dialog lookups and cascade deletes.

---

## 2. BoxButtonCollection

Ordered `vector<BoxButton*>`. Owns every `BoxButton` it holds, which in turn owns the `Data*`.

```cpp
BoxButton& bb = collection.create(myData); // takes ownership
collection.remove(bb);                      // deletes BoxButton + Data
collection.remove(myData);                  // same, by Data pointer
collection.populateBox(box);                // fills GTK flow box
collection.reindex(box);                    // re-syncs order to visual order
collection.wipe();                          // deletes all, clears vector
collection.swap(other);                     // O(1) contents exchange
```

`create(Data*)` is the **only** correct way to hand a `Data*` to a collection.

### `isSet` vs `isIdSet`

```cpp
collection.isSet(myData);      // checks by createUniqueId() — covers duplicate detection
collection.isIdSet("someId");  // same, by string
```

Both use ID comparison, not pointer comparison, so they correctly detect would-be duplicates.

### `swap`

Exchanges internal `BoxButton` vectors in O(1). `CollectionHandler` dependency registrations are keyed against the collection's **address**, so they remain valid after a swap.

---

## 3. CollectionHandler

Named singleton factory. Each name maps to one independent instance holding:
- `collection` — `StringDataMap` keyed by `createUniqueId()`. Does **not** own pointers.
- `dependencies` — `BoxButtonCollection*` targets for cascade deletes.
- `comboBoxes` — `Gtk::ComboBoxText*` auto-refreshed on `add`/`remove`.

```cpp
CollectionHandler* ch = CollectionHandler::getInstance(COLLECTION_ELEMENT);
ch->add(myData);
ch->isSet(myData);
Data* found = ch->get("myId");  // nullptr if not found — always check
```

`add()` is idempotent — it silently skips items already registered.
`remove()` guards against empty or unknown IDs — safe to call unconditionally.

---

## 4. Global vs Scoped Collections

**Global** — meaningful across the entire project, plain constants:

```cpp
COLLECTION_ELEMENT, COLLECTION_GROUP, COLLECTION_ANIMATIONS, COLLECTION_INPUT
```

**Scoped** — valid only within one parent, name = base constant + parent ID:

```cpp
string id = COLLECTION_INPUT_SOURCES + inputData->getProperties().getValue(UID);
CollectionHandler::getInstance(id)->add(mySource);
```

Scoped collections are created lazily on first `getInstance()` call and persist until `purgeAll()`. The owning `Data` subclass is responsible for registering on construction and unregistering in its destructor.

> Global: item reachable from multiple places. Scoped: item only makes sense within one parent.

---

## 5. Adding, Removing, and Replacing Items

```cpp
ch->add(myData);             // indexes by createUniqueId(); refreshes combos; no-op if already present
ch->remove(myData);          // removes, cascades Links, refreshes combos; no-op if not present
ch->replace(myData, oldId);  // re-keys if ID changed; no cascade
```

`replace` is a no-op when the new ID equals the old one.

---

## 6. Dependencies — Cascade Deletes

A `Dependency` registers a `BoxButtonCollection` so that when an item is removed from the registry, all matching entries are removed from that collection automatically.

```cpp
struct Dependency {
    BoxButtonCollection*  collection;
    size_t                minSize = 0;       // 0 = no guard
    std::function<void()> onDepletion;       // fired when size drops below minSize
};
```

**Plain** — cascade only:

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDependency({&maps});
```

**Guarded** — cascade + depletion callback:

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDependency({
    &elements,
    1,
    [this]() { CollectionHandler::getInstance(COLLECTION_GROUP)->remove(this); }
});
```

The callback fires after the full cascade completes, preventing re-entrant modification.

---

## 7. ComboBox Subscribers

```cpp
ch->registerComboBox(myCombo);   // auto-refreshed on every add/remove
ch->refreshComboBox(myCombo);    // manual refresh (e.g. on stack page switch)
```

---

## 8. Releasing Registrations

Every `registerDependency` and `registerComboBox` call must have a paired `release` in the owning object's destructor:

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&maps);
CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(myCombo);
```

---

## 9. Querying the Registry

```cpp
Data*  found = ch->get("myId");                       // nullptr if not found
bool   has   = ch->isSet(myData);                     // by createUniqueId()
bool   hasId = ch->isIdSet("someId");
size_t n     = ch->countByKey(TYPE, "1");
vector<Data*> v = ch->findByProperty(PID, parentId);
```

---

## 10. Lifecycle and purgeAll

`CollectionHandler` does not own `Data*` pointers. Correct teardown order:
1. Destroy all `BoxButtonCollection`s — recursively destroys all `Data`, which call `unregisterFromCollection()` during destruction.
2. Call `CollectionHandler::purgeAll()` to clean up the now-empty instances.

Reversing this order leaves dangling pointer calls in `Data` destructors.

---

## 11. Quick Reference

### BoxButtonCollection

| Method | Effect |
|--------|--------|
| `create(Data*)` | Takes ownership, registers to collection, returns `BoxButton&`. |
| `remove(BoxButton&)` | Deletes `BoxButton` and its `Data`; unregisters from collection. |
| `remove(Data*)` | Finds and deletes by `operator==`. |
| `populateBox(box)` | Adds all buttons to a GTK flow box. |
| `reindex(box)` | Re-syncs order to visual order. |
| `wipe()` | Deletes all, clears vector. |
| `swap(other)` | O(1) contents exchange; dependency addresses stay valid. |
| `getSize()` | Number of items. |
| `isSet(Data*)` | Checks by `createUniqueId()`. |
| `isIdSet(string)` | Checks by id string. |

### CollectionHandler

| Method | Effect |
|--------|--------|
| `getInstance(name)` | Returns (or creates) the named instance. |
| `add(Data*)` | Indexes by `createUniqueId()`; refreshes combos; no-op if already present. |
| `remove(Data*)` | Removes; cascades; refreshes combos; no-op if id empty or not present. |
| `replace(Data*, oldId)` | Re-keys if id changed; no cascade. |
| `get(id)` | Returns `Data*` or `nullptr`. |
| `isSet(Data*)` | Existence check by `createUniqueId()`. |
| `isIdSet(string)` | Existence check by id string. |
| `countByKey(key, value)` | Count items where `getValue(key) == value`. |
| `findByProperty(prop, value)` | Returns all items with matching property. |
| `registerDependency(Dependency)` | Registers a cascade target. |
| `registerComboBox(combo)` | Auto-refreshes combo on add/remove. |
| `release(BoxButtonCollection*)` | Removes a dependency registration. |
| `release(ComboBoxText*)` | Removes a combo registration. |
| `refreshComboBox(combo)` | Manually repopulates one combo. |
| `purgeAll()` | Destroys all named instances. |
