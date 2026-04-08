# LEDSpicerUI — Data System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.13 / data format 1.1.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Class Hierarchy](#2-class-hierarchy)
3. [Basic Data Storage — `values`](#3-basic-data-storage--values)
4. [Runtime Properties — `properties`](#4-runtime-properties--properties)
5. [Data That Stores Data (Composition)](#5-data-that-stores-data-composition)
6. [Links — `Link`](#6-links--link)
7. [Selection — `Selection`](#7-selection--selection)
8. [Ignoring Fields During XML Serialization](#8-ignoring-fields-during-xml-serialization)
9. [Revertible — Snapshot and Restore](#9-revertible--snapshot-and-restore)
10. [Visual and Textual Decoration](#10-visual-and-textual-decoration)
11. [Ownership — BoxButton and BoxButtonCollection](#11-ownership--boxbutton-and-boxbuttoncollection)
12. [Use Cases by Concrete Type](#12-use-cases-by-concrete-type)
13. [Quick Reference — Virtual Methods to Override](#13-quick-reference--virtual-methods-to-override)

---

## 1. Overview

`Data` is the central storage unit. Every piece of configuration is an instance of a `Data` subclass.
- `Data` inherits `Values`, which holds the serializable map and all field accessors.
- `Data` only **holds** values. All display and dialog logic lives elsewhere.
- Persistent values (serialized to XML) live in `values`.
- Runtime-only state that must never be serialized lives in `properties`.
- A `Data*` is always owned by exactly one `BoxButton`, which is owned by a `BoxButtonCollection`.

---

## 2. Class Hierarchy

```
Values
└── Data
    ├── Link               — Wraps a pointer to another Data; delegates identity.
    ├── Parent             — Adds named child BoxButtonCollection storage.
    │   ├── Device         (+ Revertible mixin) — Owns an Element collection.
    │   ├── Restrictor     (+ Revertible mixin) — Owns a RestrictorMap collection.
    │   ├── InputSource    (+ Revertible mixin) — Owns a maps collection.
    │   └── FileNode       (+ DirNode mixin)    — File-based items; FILENAME stored as property.
    │       ├── Input      (+ Revertible mixin)
    │       ├── Animation  (+ Revertible mixin) [pending]
    │       └── Profile
    ├── DirNode            — Pure structural mixin; parent pointer + path resolution.
    │   └── DirectoryEntry — Runtime-only directory node. Never serialized.
    ├── Group
    ├── Element
    ├── InputMap
    ├── InputMapLink
    └── ...
```

`Revertible` is a **pure mixin** — it does not appear in the `Data` inheritance chain.
It receives a reference to the consumer's own `values` and optionally its `children` map at construction.

---

## 3. Basic Data Storage — `values`

`values` (inherited from `Values`) is a `StringUMap` mapping field names to string values.
Everything in it is serialized to XML via `toXML()`.

```cpp
data->setValue(NAME, "MyDevice");
string name = data->getValue(NAME);
string port = data->getValue(PORT, "auto");  // default if missing
```

**`wipe()`** — unregisters from the collection handler, then clears all fields.

---

## 4. Runtime Properties — `properties`

Properties carry extra information the object needs at runtime but that has no place in the serialized config — stable identifiers, state flags, anything the object needs to track independently of the config values.

```cpp
data->getProperties().setValue(UID, "file_3");
string id  = data->getProperties().getValue(UID);
bool   has = data->getProperties().isSet(UID);
```

> `values` → XML config. `properties` → runtime information.

---

## 5. Data That Stores Data (Composition)

`Parent` subclasses own child `BoxButtonCollection` members keyed by collection ID. The constructor receives a `vector<string>` of child collection IDs to pre-create.

```cpp
Device::Device(StringUMap& data) noexcept :
    Parent(data, COLLECTION_DEVICES, {COLLECTION_ELEMENT}),
    Revertible(values, &children)
{}
```

Child collections are destroyed with the parent, recursively deleting all owned `BoxButton`s and `Data` objects.

`getChild(key)` returns the `BoxButtonCollection*` for that key, or `nullptr` if absent.

---

## 6. Links — `Link`

`Link` holds a raw pointer to another `Data` and delegates display and identity to it.

```cpp
StringUMap empty;
auto* link = new Storage::Link(empty, {"element", "name", targetData});
```

| Method | Behaviour |
|--------|-----------|
| `getCssClass()` | Delegates to the target. |
| `createPrettyName()` | Delegates to the target. |
| `createUniqueId()` | Delegates to the target. |
| `getValue(key)` | Returns target's value if key matches `linkInfo.key`; otherwise own `values`. |
| `toXML()` | Emits a self-closing XML element with the target's unique ID as the key attribute, plus any extra values in own `values`. |
| `operator==` | True if same object identity **or** if compared against the target pointer. |

---

## 7. Selection — `Selection`

Lightweight `Gtk::FlowBoxChild` used exclusively by `DialogSelect`. Holds a `Data*` without owning it. Rebuilt from scratch every time `DialogSelect` opens.

---

## 8. Ignoring Fields During XML Serialization

Override `shouldSerialize(key, value)` to suppress specific fields from XML output. The base implementation returns `true` for all fields. Prefer local logic inside the override rather than mutating any shared state.

```cpp
bool MyData::shouldSerialize(const string& key, const string& value) const noexcept {
    if (key == BRIGHTNESS and value == "100") return false;
    return true;
}
```

`toXML()` in `Data` calls `shouldSerialize()` for every entry in `values` before building the XML attribute list.

---

## 9. Revertible — Snapshot and Restore

`Revertible` is a **pure mixin**. It is not a `Data` subclass. Consumers pass a reference to their own `values` and, optionally, their `children` map at construction:

```cpp
Device::Device(StringUMap& data) noexcept :
    Parent(data, COLLECTION_DEVICES, {COLLECTION_ELEMENT}),
    Revertible(values, &children)
{}
```

| Method | Effect |
|--------|--------|
| `swap()` | Moves `values` and all registered child collections into snapshot storage. No-op if already snapped or `values` is empty. |
| `revert()` | Swaps back and wipes orphaned snapshot children. No-op if no snapshot. |
| `clearSnap()` | Discards snapshot without touching live fields or children. |

**Consumer contract:**
- Call `clearSnap()` inside `wipe()` **before** `Data::wipe()`.
- Call `revert()` inside `tearDown()` **before** `Data::tearDown()`.

```cpp
void Device::wipe() noexcept {
    clearSnap();
    Data::wipe();
}
void Device::tearDown() noexcept {
    revert();
    Data::tearDown();
}
```

---

## 10. Visual and Textual Decoration

| Method | Purpose |
|--------|---------|
| `getCssClass()` | CSS class applied to the `BoxButton` widget. |
| `createPrettyName()` | Human label shown on the button. |
| `createTooltip()` | Hover text. Default `""`. |

Call `boxButton.updateLabel()` after any `setValue()` that affects the label.

---

## 11. Ownership — BoxButton and BoxButtonCollection

```
BoxButtonCollection
 └─ BoxButton          (heap, owned by collection)
     └─ Data*          (heap, owned by BoxButton)
```

`create(Data*)` is the only correct way to hand a `Data*` to a collection. Never `delete` a `Data*` that has been passed to `create()`.

Registration in `CollectionHandler` is driven entirely by `BoxButton`:
- **Constructor** — calls `registerToCollection()` on the owned `Data`.
- **Destructor** — calls `unregisterFromCollection()` on the owned `Data`.

When a primary key changes mid-edit, call `syncRegistration(oldId)` manually after updating the field value — it re-keys the entry in the handler without destroying or recreating the `BoxButton`.

`BoxButtonCollection::swap(other)` exchanges internal contents in O(1).

### `toXML()`

`Data::toXML()` is a **non-virtual orchestrator**. It filters `values` through `shouldSerialize()`, then calls the virtual `xmlBody()` hook for inner content:

```cpp
// Self-closing (no body):
// No override needed — default xmlBody() returns "".

// Element with inner content — override xmlBody():
string MyData::xmlBody() const noexcept {
    string r;
    for (const auto& bb : *getChild(COLLECTION_FOO))
        r += bb->getData()->toXML();
    return r;
}
```

`FileNode` subclasses override `toXML()` directly, wrapping with `XMLHelper::xmlHeader()` / `XMLHelper::xmlFooter()` because they represent standalone files rather than embedded elements.

---

## 12. Use Cases by Concrete Type

| Type | `values` | Properties | Child collections | Serialized |
|------|----------|------------|-------------------|------------|
| `Device` | name, port, id, … | — | `elements` | Yes — `<device>` |
| `Element` | name, pin, position, … | strip descriptor (when strip) | — | Yes — `<element>` |
| `Group` | name, defaultColor | — | link→Element items | Yes — `<group>` |
| `Input` | input type key | `FILENAME`, `UID`, `PID` | `sources`, `linkmaps` | Yes — per-file XML |
| `Animation` | animation type key | `FILENAME`, `UID`, `PID` | (pending) | Yes — per-file XML |
| `Profile` | backgroundcolor, … | `FILENAME`, `UID`, `PID` | elements, groups, inputs, animations | Yes — per-file XML |
| `InputSource` | `source` (hw path) | `UID`, `PID`, `SOURCELESS` | `maps` | Yes — `<maps>` |
| `InputMap` | trigger, type, target, … | — | — | Yes — `<map>` |
| `DirectoryEntry` | name (segment only) | `UID` | `contents` | **No** — runtime only |
| `Link` | extra attributes | — | — | Yes — self-closing element |

---

## 13. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `getCssClass()` | **Yes** (pure) | CSS class for the `BoxButton`. |
| `getXmlTag()` | **Yes** (pure) | XML element name. |
| `createPrettyName()` | Recommended | Human label. Default uses primary key field. |
| `createTooltip()` | Optional | Hover text. Default `""`. |
| `createUniqueId()` | Recommended | Stable collection key. Default hashes primary key. |
| `xmlBody()` | When has serializable children | Inner XML content. Default `""` (self-closing). |
| `toXML()` | `FileNode` subclasses only | Full file serialization via `XMLHelper::xmlHeader/Footer`. |
| `shouldSerialize(key, value)` | When fields need suppression | Return `false` to omit a field from XML. |
| `wipe()` | When `Revertible` | Call `clearSnap()` then `Data::wipe()`. |
| `tearDown()` | When `Revertible` | Call `revert()` then `Data::tearDown()`. |
| `getPrimaryKey()` | When primary key ≠ `name` | Drives default `createUniqueId()`. |
| `getValue()` | `Link` only | Redirect key lookups to target. |
