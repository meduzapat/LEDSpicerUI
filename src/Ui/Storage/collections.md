# LEDSpicerUI — Collections System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.7 / data format 1.1.

---

## Table of Contents

1. [Overview — Two Collection Types](#1-overview--two-collection-types)
2. [BoxButtonCollection — Ordered UI Collection](#2-boxbuttoncollection--ordered-ui-collection)
3. [CollectionHandler — Global Named Registry](#3-collectionhandler--global-named-registry)
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

The system uses two distinct but complementary collection classes. They serve
different roles and should not be confused.

| | `BoxButtonCollection` | `CollectionHandler` |
|---|---|---|
| **Lives** | Inside a `Data` object or a dialog | Global static registry |
| **Purpose** | Ordered, UI-linked list of items | Keyed lookup and cascade management |
| **Keyed by** | Insertion order | `Data::createUniqueId()` string |
| **Owns `Data`?** | Yes — via `BoxButton` | No — raw pointer only |
| **One per** | Each parent `Data` or dialog | Each named collection |

A `Data` item typically appears in **both**: its `BoxButton` lives in a
`BoxButtonCollection` (where the UI display is managed), and a raw pointer to
the same `Data` object is registered in a `CollectionHandler` (where cross-dialog
lookups and cascade deletes are managed).

---

## 2. BoxButtonCollection — Ordered UI Collection

`BoxButtonCollection` is an ordered `vector<BoxButton*>`. It owns every
`BoxButton` it holds, which in turn owns the `Data*` it wraps. Destroying a
`BoxButtonCollection` destroys all its `BoxButton`s and all their `Data` objects.

### Typical home

A `BoxButtonCollection` lives either as a top-level member of `MainWindow` (for
primary dialogs) or as an embedded member of a `Data` subclass (for secondary
dialogs):

```cpp
// MainWindow — top-level collections
BoxButtonCollection devices, restrictors, groups, profiles;

// Device — embedded child collection
class Device : public Data {
    BoxButtonCollection elements;
};
```

### Core operations

```cpp
// Create a BoxButton wrapping the given Data* and append it.
// The collection takes ownership.
BoxButton& bb = collection.create(myData);

// Remove and delete a BoxButton (and its Data) by reference.
collection.remove(bb);

// Remove and delete by Data pointer.
collection.remove(myData);

// Populate a display box with all current items.
collection.populateBox(box);

// Re-sync internal order to match the current visual order in the box.
collection.reindex(box);

// Delete all BoxButtons and their Data, then clear the vector.
collection.wipe();
```

### Ownership rule

`BoxButtonCollection::create(Data*)` is the **only** correct way to hand a
`Data*` to a collection. After that call the collection owns the pointer. Never
`delete` a `Data*` that has been passed to `create()`.

`wipe()` deletes every BoxButton and its Data, then clears the vector. Use it when you want to discard all items unconditionally — the GTK side must be wiped separately via OrdenableFlowBox::wipe() first to detach the widgets before their backing objects are destroyed.

---

## 3. CollectionHandler — Global Named Registry

`CollectionHandler` is a **named singleton factory**. Each name maps to one
independent instance. All instances live for the lifetime of the application
and are destroyed together by `purgeAll()`.

```cpp
// Always returns the same instance for a given name.
CollectionHandler* ch = CollectionHandler::getInstance(COLLECTION_ELEMENT);
```

Internally each instance holds:

- `collection` — a `StringDataMap` (`std::map<string, Data*>`) keyed by
  `createUniqueId()`. Does **not** own the pointers.
- `dependencies` — a list of `BoxButtonCollection*` that hold `Link`s pointing
  into this registry. Used for cascade deletes.
- `comboBoxes` — a list of `Gtk::ComboBoxText*` that display this registry's
  items. Auto-refreshed on every `add` / `remove`.

### Registration and lookup

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->add(myElement);
CollectionHandler::getInstance(COLLECTION_ELEMENT)->isSet(myElement);    // by pointer
CollectionHandler::getInstance(COLLECTION_ELEMENT)->isIdSet("myId");     // by key
Data* found = CollectionHandler::getInstance(COLLECTION_ELEMENT)->get("myId");
```

`add()` calls `refreshComboBoxes()` automatically — any registered combo box
gets updated immediately.

`get()` returns `nullptr` if the id is not found; always check the return value.

---

## 4. Global vs Scoped Collections

Some collection names are **global** — they hold items that are meaningful
across the entire project (elements, groups, animations, inputs). Their names
are plain constants:

```cpp
COLLECTION_ELEMENT    // "elements"
COLLECTION_GROUP      // "groups"
COLLECTION_ANIMATIONS // "animations"
COLLECTION_INPUT      // "inputs"
```

Other collections are **scoped** — they hold items that only make sense in the
context of one specific parent. Their names are constructed at runtime by
concatenating a base constant with a parent identifier:

```cpp
// Sources belonging to a specific input file, keyed by the file's FILE_ID property.
string collectionId = COLLECTION_INPUT_SOURCES + inputData->getProperty(FILE_ID);
CollectionHandler::getInstance(collectionId)->add(mySource);

// Maps belonging to a specific input source.
string mapsId = COLLECTION_INPUT_MAPS + ownerData->createUniqueId();
CollectionHandler::getInstance(mapsId)->add(myMap);
```

Scoped collections are created lazily on first `getInstance()` call and persist
until `purgeAll()`. A `Data` subclass that uses a scoped collection is
responsible for registering itself on construction/`activate()` and unregistering
in its destructor.

### Rule

> If the item should be reachable from dialogs other than its direct parent
> → global collection.
> If the item only makes sense within one specific parent instance
> → scoped collection, name = base constant + parent unique id or property.

---

## 5. Adding, Removing, and Replacing Items

### add

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->add(myData);
```

Indexes the pointer by `myData->createUniqueId()`. Triggers combo box refresh.
Called after `BoxButtonCollection::create()` in the dialog flow.

### remove

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->remove(myData);
```

Removes the pointer from the registry, then iterates all registered
`dependencies` and calls `BoxButtonCollection::remove(myData)` on each one.
This is the cascade: every `Link` in every dependent collection that points to
`myData` is deleted automatically. After all cascades, any registered depletion
callbacks are fired (see §6). Triggers combo box refresh.

### replace

```cpp
CollectionHandler::getInstance(COLLECTION_ELEMENT)->replace(myData, oldId);
```

Used after an item is renamed (its `createUniqueId()` changes). If the new ID
equals the old one, it is a no-op. Otherwise it erases the old key and inserts
under the new one. Does **not** cascade — the `Data*` itself does not change,
only its registry key.

Called by `DialogForm::onEditClicked` after `storeData()`.

---

## 6. Dependencies — Cascade Deletes

A `Dependency` registers a `BoxButtonCollection` with a `CollectionHandler` so
that when an item is removed from the registry, all `Link`s pointing to it are
also removed from that collection automatically.

```cpp
struct Dependency {
    BoxButtonCollection*  collection;   // Collection to cascade into.
    size_t                minSize = 0;  // 0 = no guard.
    std::function<void()> onDepletion;  // Called when size drops below minSize.
};
```

### Plain dependency — cascade only

Register in the `Data` constructor when the collection simply needs to track
deletions from a global source:

```cpp
// InputSource.cpp — its maps reference elements and groups globally.
// When an element or group is deleted, remove any map that referenced it.
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDependency({&maps});
CollectionHandler::getInstance(COLLECTION_GROUP)->registerDependency({&maps});
```

### Guarded dependency — cascade + depletion callback

Register when the parent `Data` object must be destroyed if its child collection
drops below a minimum size. `minSize = 1` means "destroy the owner when the
last child is removed":

```cpp
// Group.cpp — a group with no elements is meaningless; auto-delete it.
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDependency({
    &elements,
    1,                // minimum 1 element required
    [this]() {
        CollectionHandler::getInstance(COLLECTION_GROUP)->remove(this);
    }
});
```

The callback fires **after** the cascade iteration completes, preventing
re-entrant modification of the dependency list during iteration.

### Sequence of events when an element is deleted

```
CollectionHandler("elements")->remove(myElement)
  1. Erases myElement from collection map.
  2. For each dependency:
       dep.collection->remove(myElement)    ← removes all Links pointing to myElement
       if dep.minSize and dep.collection->getSize() < dep.minSize:
           queue dep.onDepletion
  3. refreshComboBoxes()
  4. Fire queued depletion callbacks.
        → e.g. CollectionHandler("groups")->remove(emptyGroup)
            → which itself cascades into profile collections, etc.
```

---

## 7. ComboBox Subscribers

Any `Gtk::ComboBoxText` that should always reflect the current contents of a
collection can be registered as a subscriber. It is repopulated automatically
on every `add`, `remove`, or manual `refreshComboBox` call.

```cpp
// Register once, usually in the dialog constructor.
CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerComboBox(comboBoxInputMapElement);

// Manually refresh one combo (e.g. when the visible stack page changes).
CollectionHandler::getInstance(COLLECTION_ELEMENT)->refreshComboBox(comboBoxInputMapElement);
```

`refreshComboBox` calls `remove_all()` then re-appends every item's
`createPrettyName()`. The active selection is cleared; the caller is responsible
for restoring it if needed.

---

## 8. Releasing Registrations

When a `Data` object that registered dependencies or combo boxes is destroyed,
it **must** release those registrations to prevent dangling pointer access.

```cpp
// InputSource destructor — release both global dependencies.
CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&maps);
CollectionHandler::getInstance(COLLECTION_GROUP)->release(&maps);
```

`release(BoxButtonCollection*)` finds and removes the matching `Dependency`
entry by pointer comparison. `release(Gtk::ComboBoxText*)` does the same for
combo boxes.

### Rule

> Every `registerDependency` or `registerComboBox` call must have a paired
> `release` call in the owning object's destructor.

The typical pattern in a `Data` subclass:

```cpp
MyData::MyData(StringUMap& data) : Data(data) {
    CollectionHandler::getInstance(COLLECTION_ELEMENT)->registerDependency({&myChildren});
}

MyData::~MyData() {
    CollectionHandler::getInstance(COLLECTION_ELEMENT)->release(&myChildren);
    if (CollectionHandler::getInstance(COLLECTION_MY_THINGS)->isSet(this))
        CollectionHandler::getInstance(COLLECTION_MY_THINGS)->remove(this);
}
```

---

## 9. Querying the Registry

### Exact lookup

```cpp
Data* el = CollectionHandler::getInstance(COLLECTION_ELEMENT)->get("P1_BUTTON1");
if (not el) { /* not found */ }
```

### Existence checks

```cpp
bool byPtr = ch->isSet(myData);       // compares createUniqueId()
bool byKey = ch->isIdSet("someId");
```

### Count occurrences of a field value

```cpp
// How many elements have type == "1"?
size_t n = CollectionHandler::getInstance(COLLECTION_ELEMENT)->countByKey(TYPE, "1");
```

### Find by property

```cpp
// All input sources belonging to a specific file.
vector<Data*> sources = CollectionHandler::getInstance(COLLECTION_INPUT_SOURCES + fileId)
    ->findByProperty(FILE_ID, fileId);
```

### Iteration

```cpp
for (const auto& [id, data] : *CollectionHandler::getInstance(COLLECTION_ELEMENT)) {
    // id  = createUniqueId() string
    // data = Data* pointer
}
```

---

## 10. Lifecycle and purgeAll

`CollectionHandler` instances are created lazily and live until
`CollectionHandler::purgeAll()` is called, which deletes every instance and
clears the static map.

```cpp
// Called at project close / app shutdown.
CollectionHandler::purgeAll();
```

Since `CollectionHandler` does not own the `Data*` pointers it holds, `purgeAll`
does **not** delete `Data` objects. Data is owned by `BoxButton`s, which are
owned by `BoxButtonCollection`s, which are destroyed when their owning `Data`
or dialog is destroyed. The correct teardown order is:

1. Destroy the top-level `BoxButtonCollection`s (devices, groups, etc.) — this
   recursively destroys all child `Data` objects, which call `remove()` on
   their `CollectionHandler`s during destruction.
2. Call `purgeAll()` to clean up the now-empty `CollectionHandler` instances
   themselves.

Reversing this order leaves dangling pointer calls in `Data` destructors.

---

## 11. Quick Reference

### BoxButtonCollection

| Method | Effect |
|--------|--------|
| `create(Data*)` | Takes ownership, returns `BoxButton&`. |
| `remove(BoxButton&)` | Deletes `BoxButton` and its `Data`. |
| `remove(Data*)` | Finds and deletes by pointer equality. |
| `populateBox(box)` | Adds all buttons to a GTK flow box. |
| `reindex(box)` | Re-syncs internal order to visual order. |
| `wipe()` | Deletes all BoxButtons and their `Data`, then clears. |
| `getSize()` | Number of items. |
| `isSet(Data*)` | Checks by `createUniqueId()`. |
| `isIdSet(string)` | Checks by id string. |

### CollectionHandler

| Method | Effect |
|--------|--------|
| `getInstance(name)` | Returns (or creates) the named instance. |
| `add(Data*)` | Indexes by `createUniqueId()`; refreshes combos. |
| `remove(Data*)` | Removes from registry; cascades; refreshes combos. |
| `replace(Data*, oldId)` | Re-keys if id changed; no cascade. |
| `get(id)` | Returns `Data*` or `nullptr`. |
| `isSet(Data*)` | Existence check by pointer. |
| `isIdSet(string)` | Existence check by id string. |
| `countByKey(key, value)` | Count items where `getValue(key) == value`. |
| `findByProperty(prop, value)` | Returns all items with matching property. |
| `registerDependency(Dependency)` | Registers a cascade target. |
| `registerComboBox(combo)` | Auto-refreshes combo on add/remove. |
| `release(BoxButtonCollection*)` | Removes a dependency registration. |
| `release(ComboBoxText*)` | Removes a combo registration. |
| `refreshComboBox(combo)` | Manually repopulates one combo. |
| `purgeAll()` | Destroys all named instances. |