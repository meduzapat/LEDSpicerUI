# LEDSpicerUI — Data System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.10 / data format 1.1.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Class Hierarchy](#2-class-hierarchy)
3. [Basic Data Storage — `fieldsData`](#3-basic-data-storage--fieldsdata)
4. [Runtime Properties — `properties`](#4-runtime-properties--properties)
5. [Data That Stores Data (Composition)](#5-data-that-stores-data-composition)
6. [Links — `Link`](#6-links--link)
7. [Selection — `Selection`](#7-selection--selection)
8. [Activation and Deactivation](#8-activation-and-deactivation)
9. [Revertible — Snapshot and Restore](#9-revertible--snapshot-and-restore)
10. [Visual and Textual Decoration](#10-visual-and-textual-decoration)
11. [Ignoring Fields During XML Serialization](#11-ignoring-fields-during-xml-serialization)
12. [Ownership — BoxButton and BoxButtonCollection](#12-ownership--boxbutton-and-boxbuttoncollection)
13. [Use Cases by Concrete Type](#13-use-cases-by-concrete-type)
14. [Quick Reference — Virtual Methods to Override](#14-quick-reference--virtual-methods-to-override)

---

## 1. Overview

`Data` is the central storage unit. Every piece of configuration is an instance of a `Data` subclass.
- `Data` only **holds** values. All display and dialog logic lives elsewhere.
- Persistent values (serialized to XML) live in `fieldsData`.
- Runtime-only state that must never be serialized lives in `properties`.
- A `Data*` is always owned by exactly one `BoxButton`, which is owned by a `BoxButtonCollection`.

---

## 2. Class Hierarchy

```
Data
├── Link               — Wraps a pointer to another Data; delegates identity.
├── Revertible         — Adds snap/restore capability.
│   ├── DirNode        — Base for tree-navigable items (path-aware).
│   │   ├── DirectoryEntry — Runtime-only directory node. Never serialized.
│   │   └── FileData   — File-based items; stores FILENAME as a property.
│   │       ├── Input
│   │       ├── Animation
│   │       └── Profile
│   ├── Device         — Owns an Element collection.
│   ├── Restrictor     — Owns a RestrictorMap collection.
│   └── InputSource    — Owns a maps collection.
├── Group
├── Element
├── InputMap
├── InputMapLink
└── ...
```

---

## 3. Basic Data Storage — `fieldsData`

`fieldsData` is a `StringUMap` mapping field names to string values. Everything in it is serialized to XML via `toXML()`.

```cpp
data->setValue(NAME, "MyDevice");
string name = data->getValue(NAME);
string port = data->getValue(PORT, "auto");  // default if missing
```

**`wipe()`** — clears all serializable fields.
**`reset()`** — clears fields and unregisters from active collections.

---

## 4. Runtime Properties — `properties`

Properties carry extra information the object needs at runtime but that has no place in the serialized config — stable identifiers, state flags, anything the object needs to track independently of the config values.

```cpp
setProperty(UID, "file_3");
string id = getProperty(UID);
bool  has = hasProperty(UID);
```

> `fieldsData` → XML config. `properties` → runtime information.

---

## 5. Data That Stores Data (Composition)

A `Data` subclass can own child `BoxButtonCollection` members to hold related sub-items. How those collections are populated and exposed is up to the subclass.

```cpp
class Device : public Revertible {
    BoxButtonCollection elements;
};
```

The child collection is destroyed with the parent, recursively deleting all owned `BoxButton`s and `Data` objects.

---

## 6. Links — `Link`

`Link` holds a raw pointer to another `Data` and delegates display and identity to it. Used to reference another `Data` without copying it.

```cpp
StringUMap empty;
auto* link = new Storage::Link(empty, {"element", "name", targetData});
```

| Method | Behaviour |
|--------|-----------|
| `getCssClass()` | Delegates to the target. |
| `createPrettyName()` | Delegates to the target. |
| `createUniqueId()` | Delegates to the target. |
| `getValue(key)` | Returns target's value if key matches `linkInfo.key`; otherwise own `fieldsData`. |
| `toXML()` | Emits a self-closing XML element with the target's unique ID as the key attribute, plus any extra values in own `fieldsData`. |
| `operator==` | True if same object identity **or** if compared against the target pointer. |

---

## 7. Selection — `Selection`

Lightweight `Gtk::FlowBoxChild` used exclusively by `DialogSelect`. Holds a `Data*` without owning it. Rebuilt from scratch every time `DialogSelect` opens.

---

## 8. Activation and Deactivation

`activate()` and `deActivate()` are lifecycle hooks called by the dialog system when an item is opened for editing and when the dialog closes. Override them to perform any setup or teardown the object needs during those moments.

```cpp
void MyData::activate()   { /* prepare while being edited */ }
void MyData::deActivate() { /* clean up after dialog closes */ }
```

Both default to no-ops in `Data`. Override only when needed.

---

## 9. Revertible — Snapshot and Restore

`Revertible : public Data` adds the ability to snapshot `fieldsData` and registered child collections, then restore them on demand. Useful wherever an object may be partially mutated during an operation the user can cancel.

Subclasses call `registerChild()` from their constructor body for each owned `BoxButtonCollection` that should participate in the snapshot:

```cpp
Device::Device(StringUMap& data) : Revertible(data) {
    registerChild(elements);
}
```

| Method | Effect |
|--------|--------|
| `swap()` | Moves `fieldsData` and all registered child collections into snapshot storage. No-op if already snapped or `fieldsData` is empty. |
| `restore()` | Swaps back and wipes orphaned snapshot children. No-op if no snapshot. |
| `wipe()` | Discards snapshot and clears `fieldsData`. |
| `deActivate()` | Calls `restore()` if a snapshot is present, then `Data::deActivate()`. |

---

## 10. Visual and Textual Decoration

| Method | Purpose |
|--------|---------|
| `getCssClass()` | CSS class applied to the `BoxButton` widget. |
| `createPrettyName()` | Human label shown on the button. |
| `createTooltip()` | Hover text. Default `""`. |

Call `boxButton.updateLabel()` after any `setValue()` that affects the label.

---

## 11. Ignoring Fields During XML Serialization

The `ignored` member (`StringUSet`, `mutable`) lists field keys that exist in `fieldsData` but should be omitted from XML output. It predates `properties` and was originally used to suppress fields stored in `fieldsData` for UI convenience that were optional or had omittable defaults in the config.

Current use: suppress fields whose value is empty or equal to a known default the loader handles implicitly.

```cpp
const string Element::toXML() const {
    StringUSet ignored;
    if (fieldsData.at(BRIGHTNESS) == "100") ignored.insert(BRIGHTNESS);
    return createOpeningXML("element", fieldsData, ignored, true);
}
```

Prefer a local `StringUSet` inside `toXML()` for conditional logic rather than mutating the member.

---

## 12. Ownership — BoxButton and BoxButtonCollection

```
BoxButtonCollection
 └─ BoxButton          (heap, owned by collection)
     └─ Data*          (heap, owned by BoxButton)
```

`create(Data*)` is the only correct way to hand a `Data*` to a collection. Never `delete` a `Data*` that has been passed to `create()`.

`BoxButtonCollection::swap(other)` exchanges internal contents in O(1).

### `toXML()`

Subclasses override `toXML()` to produce the XML representation of the item. Use the static helpers:

```cpp
// Self-closing element (no children):
const string MyData::toXML() const {
    return createOpeningXML("myData", fieldsData, ignored, true);
}
// Element with children:
const string MyData::toXML() const {
    string r(createOpeningXML("myData", fieldsData, ignored, false));
    for (const auto& child : children)
        r += child->getData()->toXML();
    return r + createClosingXML("myData");
}
```

`valuesXML()` chooses inline or multi-line attribute layout automatically based on the number of fields.

---

## 13. Use Cases by Concrete Type

| Type | `fieldsData` | Properties | Child collections | Serialized |
|------|-------------|------------|-------------------|------------|
| `Device` | name, port, id, … | — | `elements` | Yes — `<device>` |
| `Element` | name, pin, position, … | strip descriptor (when strip) | — | Yes — `<element>` |
| `Group` | name | — | Link→Element items | Yes — `<group>` |
| `Input` | name (input type) | `FILENAME`, `UID` | `sources` | Yes — per-file XML |
| `InputSource` | `source` (hw path) | `UID`, `PID`, `SOURCELESS` | `maps` | Yes — `<maps>` |
| `InputMap` | trigger, type, target, … | — | — | Yes — `<map>` |
| `DirectoryEntry` | name (segment only) | `UID` | `contents` | **No** — runtime only |
| `FileData` | name (item type) | `FILENAME`, `UID` | — | Yes |
| `Link` | extra attributes | — | — | Yes — self-closing element |

---

## 14. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `getCssClass()` | **Yes** (pure) | CSS class for the `BoxButton`. |
| `createPrettyName()` | Recommended | Human label. Default uses primary key field. |
| `createTooltip()` | Optional | Hover text. Default `""`. |
| `createUniqueId()` | Recommended | Stable collection key. Default hashes primary key. |
| `toXML()` | Recommended | XML output. Default emits flat attributes. |
| `activate()` | When needed | Lifecycle hook on edit open. |
| `deActivate()` | When needed | Lifecycle hook on edit close. |
| `wipe()` | When has children | Clears all serializable fields. |
| `reset()` | When wipe must unregister | Clears fields and unregisters from active collections. |
| `getPrimaryKey()` | When primary key ≠ `name` | Drives default `createUniqueId()`. |
| `getValue()` | `Link` only | Redirect key lookups to target. |
