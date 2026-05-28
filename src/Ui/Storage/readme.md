# LEDSpicerUI — Data System Developer Guide

---

## Table of Contents

1.  [Overview](#1-overview)
2.  [Class Hierarchy](#2-class-hierarchy)
3.  [Values — The Field Store](#3-values--the-field-store)
4.  [Data — The Config Unit](#4-data--the-config-unit)
5.  [Runtime Properties](#5-runtime-properties)
6.  [Data That Stores Data (Composition)](#6-data-that-stores-data-composition)
7.  [Links — `Link`](#7-links--link)
8.  [Selection — `Selection`](#8-selection--selection)
9.  [Ignoring Fields During XML Serialization](#9-ignoring-fields-during-xml-serialization)
10. [DirNode — Directory Tree Mixin](#10-dirnode--directory-tree-mixin)
11. [Visual and Textual Decoration](#11-visual-and-textual-decoration)
12. [Ownership — BoxButton and BoxButtonCollection](#12-ownership--boxbutton-and-boxbuttoncollection)
13. [Use Cases by Concrete Type](#13-use-cases-by-concrete-type)
14. [Quick Reference — Virtual Methods to Override](#14-quick-reference--virtual-methods-to-override)

---

## 1. Overview

`Data` is the central storage unit. Every piece of configuration is an instance of a `Data` subclass.

- `Values` is the base field store. It holds a `StringUMap` and exposes all field accessors. Both `Data` (via inheritance) and `Data::properties` (as a member) use `Values` directly.
- `Data` inherits `Values` for its serializable fields and adds identity, XML output, and collection registration.
- Persistent values (serialized to XML) live in `values` (inherited from `Values`).
- Runtime-only state lives in `properties` (a `Values` member inside `Data`).
- A `Data*` is always owned by exactly one `BoxButton`, which is owned by a `BoxButtonCollection`.

---

## 2. Class Hierarchy

```
Values                         — General-purpose field store. Used directly wherever a plain key-value map is needed.
└── Data                       — Central config unit. Adds identity, XML, collection registration.
    ├── Link                   — Wraps a pointer to another Data; delegates identity.
    │   └── InputMap           — Link with fixed linkKey=TARGET, linkType=TYPE_MAP.
    ├── Parent                 — Adds named child BoxButtonCollection storage.
    │   ├── Device             — Owns an Element collection.
    │   ├── Restrictor         — Owns a RestrictorMap collection.
    │   ├── InputSource        — Owns a maps collection.
    │   ├── Group              — Owns a Link→Element collection.
    │   ├── InputMapLink       — Owns an InputMap-link collection (no XML tag of its own).
    │   ├── Input              (+ DirNode mixin) — File-based; owns sources and link maps.
    │   ├── Animation          (+ DirNode mixin) — File-based; owns an Actor collection.
    │   └── Profile            (+ DirNode mixin) — File-based; owns scoped elements/groups/inputs/animations.
    ├── Element
    ├── Actor                  — Single animation actor (Filler, Pulse, Audio, …); scoped to a parent Animation by PID.
    ├── Process                — Standalone process entry; primary key is PARAM_PROCESS_NAME.
    └── DirectoryEntry         (+ DirNode mixin) — Runtime-only directory node. Never serialized.
```

**Mixins — not in the `Data` inheritance chain:**

| Mixin | Consumer | Purpose |
|-------|----------|---------|
| `DirNode` | `Input`, `Animation`, `Profile`, `DirectoryEntry` | Parent pointer, UID/PID/FILENAME written into a `Values` dest. |

---

## 3. Values — The Field Store

`Values` (defined in `src/Values.hpp`, one level above `Storage/`) is a standalone class that owns a `StringUMap` and exposes all field accessors. It is not tied to `Data` — any class that needs a plain key-value store can use it directly. Current known consumers:

- **`Data` base** — the serializable field map inherited by every `Data` subclass.
- **`Data::properties`** — a `Values` member inside every `Data` for runtime-only state.
- **`DirNode` constructor** — takes a `Values&` destination to write `UID`, `PID`, `FILENAME` into.
- **`Parent::children`** — child `BoxButtonCollection`s are keyed by collection ID string.

### API

```cpp
values.getValue(key);                  // returns const string& or emptyString
values.getValue(key, defaultValue);    // returns string
values.setValue(key, value);           // overloads: string, const char*, int,
                                       // unsigned, long, unsigned long, double, bool
values.isSet(key);                     // true if key exists
values.unSet(key);                     // removes key
values.wipe();                         // clears all entries
values.getValues();                    // returns const StringUMap*
values.setValues(other);               // inserts all entries from other
values.copyValues();                   // returns StringUMap copy
values.swap(other);                    // O(1) contents exchange
// Iterators: begin(), end(), cbegin(), cend()

// Typed reads — best-effort parse of the stored string.
values.isNumber(key);                  // true if std::stod accepts the value
values.getInt(key);                    // std::stoi; 0 on missing/garbage/overflow
values.getDouble(key);                 // std::stod; 0.0 on missing/garbage/overflow
values.is(key);                        // value == HUMAN_TRUE
values.isA(key, "Foo");                // value == "Foo"
```

`Values` is default-constructible (empty map) or pre-populated from a `StringUMap&` (which is moved in).

The typed setters accept a real numeric or boolean value and stringify
it on the way in; `setValue(key, true)` writes `HUMAN_TRUE`, etc. The
typed getters parse the stored string and return `0` / `0.0` (or
`false` for `is`) on any failure — they never throw. Two convenience
constants live next to `emptyString`:

```cpp
inline const string emptyString;       // ""
inline const string emptyNumber{"0"};  // "0"
```

---

## 4. Data — The Config Unit

`Data` inherits `Values` and adds:

- Identity: `createUniqueId()`, `createPrettyName()`, `createTooltip()`, `getPrimaryValue()`
- XML: `toXML()`, `getXmlTag()`, `xmlBody()`, `shouldSerialize()`
- Collection: `getCollectionHandler()`, `registerToCollection()`, `unregisterFromCollection()`, `syncRegistration()`
- Lifecycle: `setUp()`, `tearDown()`, `wipe()`, `unSet()`
- Properties: `getProperties()` → `Values&`

```cpp
data->setValue(NAME, "MyDevice");
data->setValue(BRIGHTNESS, 100);             // typed overload, no to_string needed
data->setValue(ENABLED, true);               // stores HUMAN_TRUE
string name = data->getValue(NAME);
string port = data->getValue(PORT, "auto");  // default if missing
int    pins = data->getInt(PINS);            // 0 if missing/garbage
data->wipe();                                // unregisters from handler, clears values
```

> **Loading raw data:** the typed getters (`getInt`, `getDouble`,
> `isNumber`) and `is()` are the safe way to read freshly-loaded data
> in `retrieveData()`. Raw values from XML are untrusted strings;
> direct `std::stoi(getValue(...))` will throw on garbage inside a
> `noexcept` method and crash the program.

**`getPrimaryValue()`** returns `getValue(getPrimaryKey())`. The default primary key is `NAME`. Override `getPrimaryKey()` when a subclass uses a different field as its identifier.

**`syncRegistration(oldId)`** — call after manually changing the primary key field. Re-keys the `CollectionHandler` entry without destroying the `BoxButton`.

---

## 5. Runtime Properties

Properties carry runtime-only information that must never be serialized — stable identifiers, state flags, UI-only labels, and anything the object needs independently of the config values.

```cpp
data->getProperties().setValue(UID, "file_3");
string id = data->getProperties().getValue(UID);
bool   has = data->getProperties().isSet(UID);
```

`properties` is a `Values` instance — the full `Values` API applies.

> `values` (inherited) → XML config. `properties` (member) → runtime information.

---

## 6. Data That Stores Data (Composition)

`Parent` subclasses own child `BoxButtonCollection` members keyed by collection ID. The constructor receives a `vector<string>` of child collection IDs to pre-create.

```cpp
Device::Device(StringUMap& data) noexcept :
    Parent(data, {COLLECTION_ELEMENTS})
{}
```

Child collections are destroyed with the parent, recursively deleting all owned `BoxButton`s and `Data` objects.

| Method | Purpose |
|--------|---------|
| `getChild(id)` | Returns the `BoxButtonCollection*` for that collection ID, or `nullptr`. |
| `getPrimaryChild()` | Returns the first child collection (used as default size source). |
| `getChildren()` | Returns the full `id → BoxButtonCollection` map. |
| `getSize()` | Item count of the primary child. Override when a different family is the size driver. |
| `begin()` / `end()` | Iterate over all child collections. |

### Registering dependencies on global collections

`Parent::registerDependency(watchedCollection, targetFamily)` is the
preferred way to wire cascade deletes from a global collection into one of
this `Parent`'s child families. Used by `Group`, `Profile`, `Animation`,
`InputSource`, `InputMapLink`:

```cpp
// Group.hpp — child links to global Elements are removed when an Element disappears.
registerDependency(COLLECTION_ELEMENTS, COLLECTION_GROUP_LINKS);

// Profile.cpp — every Profile child family follows its global counterpart.
registerDependency(COLLECTION_ELEMENTS,   COLLECTION_PROFILE_ELEMENTS);
registerDependency(COLLECTION_GROUPS,     COLLECTION_PROFILE_GROUPS);
registerDependency(COLLECTION_INPUTS,     COLLECTION_PROFILE_INPUTS);
registerDependency(COLLECTION_ANIMATIONS, COLLECTION_PROFILE_ANIMATIONS);
```

`Parent` resolves the watched `CollectionHandler*` and the target child
`BoxButtonCollection*` itself and forwards the pairing to the handler.
The registration is automatically released when the `Parent` is destroyed.

---

## 7. Links — `Link`

`Link` wraps a raw pointer to another `Data` (`link`) and delegates display and identity to it. It carries its own `fieldsData` (via inherited `values`) for extra per-link attributes like `color` or `filter`.

`linkKey` and `linkType` are stable `const string&` references — they must be owned by the caller for the lifetime of the `Link` (e.g. stored in `DialogSelect::SelectionRequest`). `linkFields` is a stable `const vector<LinkField>&` reference with the same lifetime requirement.

### Constructor

```cpp
StringUMap extra{{COLOR, "Red"}};
auto* link = new Storage::Link(extra, linkKey, linkType, linkFields, target);
```

- `linkKey` — field name used to identify the target in XML (e.g. `NAME`).
- `linkType` — XML tag name for this link (e.g. `"element"`, `"group"`).
- `linkFields` — editable extra fields shown in `DialogLinkEditor`; pass `{}` if none.
- `target` — the `Data` this link points to. May be `nullptr` until set via `setLink()`.

### Methods

| Method | Behaviour |
|--------|-----------|
| `getCssClass()` | Delegates to `link`. |
| `createPrettyName()` | Delegates to `link`. |
| `createUniqueId()` | Delegates to `link`. |
| `getXmlTag()` | Returns `linkType`. |
| `getValue(linkKey)` | Returns `link->getPrimaryValue()`. |
| `getValue(other)` | Returns own `values` entry. |
| `setValue(linkKey, …)` | Silently ignored — the key is owned by the target. |
| `setValue(other, …)` | Updates own `values`. |
| `toXML()` | Self-closing element: tag=`linkType`, key attribute=`linkKey`→`link->createUniqueId()`, plus own `values`. |
| `operator==` | True if same object identity **or** if compared against the `link` pointer. |
| `setLink(newLink)` | Replaces the target pointer. |
| `getLinkFields()` | Returns `const vector<LinkField>&`. |
| `getCollectionHandlerSource()` | Returns `link->getCollectionHandler()` — the collection the link was picked from. |

### `LinkField`

Describes an extra editable field shown in `DialogLinkEditor`:

```cpp
struct LinkField {
    enum class Widget : uint8_t { COLOR_PICKER, COMBOBOX };
    const string key;           // fieldsData key on the Link.
    const string label;         // Human label shown in the editor.
    const string defaultValue;  // Value used when not yet set.
    const Widget widgetType;
};
```

### `InputMap` — Link subclass

`InputMap` extends `Link` with fixed `linkKey = TARGET` and `linkType = TYPE_MAP`. It carries `trigger`, `type`, `color`, `filter` in its own `fieldsData`. Editing is handled by `DialogInputMap`, not `DialogLinkEditor`.

---

## 8. Selection — `Selection`

Lightweight `Gtk::FlowBoxChild` used exclusively by `DialogSelect`. Holds a `Data*` without owning it. Rebuilt from scratch every time `DialogSelect` opens.

---

## 9. Ignoring Fields During XML Serialization

Override `shouldSerialize(key, value)` to suppress specific fields from XML output. The base implementation skips entries whose value is empty (`return not value.empty();`) — override it when more nuanced filtering is needed.

```cpp
bool MyData::shouldSerialize(const string& key, const string& value) const noexcept {
    if (key == BRIGHTNESS and value == "100") return false; // suppress defaults
    return not value.empty();                                // keep the base behaviour
}
```

`toXML()` in `Data` calls `shouldSerialize()` for every entry in `values` before building the XML attribute list.

---

## 10. DirNode — Directory Tree Mixin

`DirNode` is a **pure mixin** — not a `Data` subclass. It provides parent pointer and recursive path resolution for objects that live inside a directory tree.

The constructor takes a `Values& dest` — the destination where `UID`, `PID`, and `FILENAME` are written. Consumers pass either `values` (for `Values`-inherited fields) or `properties` (for runtime-only identity):

```cpp
// Input / Animation / Profile — identity in properties (not serialized):
DirNode(getProperties(), parent, filename)

// DirectoryEntry — identity in properties (never serialized):
DirNode(getProperties(), parent, filename)
```

| Method | Purpose |
|--------|---------|
| `getName()` | Returns `FILENAME` from dest. |
| `getFsId()` | Returns `UID` from dest. |
| `getParent()` | Returns parent `DirNode*`. |
| `getPath()` | Full path of the parent. Empty at root. |
| `getFullPath()` | Full path including this node's name. |
| `isAtRoot()` | True if no parent. |

`UID` is a monotonically incrementing counter shared across all `DirNode` instances — unique per application run, not persistent.

---

## 11. Visual and Textual Decoration

| Method | Purpose |
|--------|---------|
| `getCssClass()` | CSS class applied to the `BoxButton` widget. |
| `createPrettyName()` | Human label shown on the button. Default: `getPrimaryValue()`. |
| `createTooltip()` | Hover text. Default `""`. |

Call `boxButton.updateLabel()` after any `setValue()` that affects the label.

---

## 12. Ownership — BoxButton and BoxButtonCollection

```
BoxButtonCollection
 └─ BoxButton          (heap, owned by collection)
     └─ Data*          (heap, owned by BoxButton)
```

`create(Data*)` is the **only** correct way to hand a `Data*` to a collection. Never `delete` a `Data*` that has been passed to `create()`.

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

File-backed `Parent` subclasses (`Input`, `Animation`, `Profile` — those that mix in `DirNode`) override `toXML()` directly, wrapping with `XMLHelper::xmlHeader()` / `XMLHelper::xmlFooter()` because they represent standalone files rather than embedded elements.

---

## 13. Use Cases by Concrete Type

| Type | `values` | `properties` | Child collections | Serialized |
|------|----------|--------------|-------------------|------------|
| `Device` | name, port, id, … | — | `COLLECTION_ELEMENTS` | Yes — `<device>` |
| `Element` | name, pin, position, … | strip descriptor (when strip) | — | Yes — `<element>` |
| `Group` | name, defaultColor | — | `COLLECTION_GROUP_LINKS` (Links → Element) | Yes — `<group>` |
| `Actor` | type-specific fields | `PID` (parent Animation), stable UID | — | Yes — `<actor>` |
| `Process` | process name, mapping data | — | — | Yes — `<map>` (under `<processLookup>`) |
| `Input` | input type key | `FILENAME`, `UID`, `PID` | `COLLECTION_INPUT_SOURCES`, `COLLECTION_INPUT_LINKMAPS` | Yes — per-file XML |
| `Animation` | — | `FILENAME`, `UID`, `PID` | `COLLECTION_ACTORS` | Yes — per-file XML |
| `Profile` | backgroundColor, … | `FILENAME`, `UID`, `PID` | `COLLECTION_PROFILE_ELEMENTS`, `COLLECTION_PROFILE_GROUPS`, `COLLECTION_PROFILE_INPUTS`, `COLLECTION_PROFILE_ANIMATIONS` | Yes — per-file XML |
| `InputSource` | `source` (hw path) | `UID`, `PID`, `SOURCELESS` | `COLLECTION_INPUT_MAPS` | Yes — `<maps>` |
| `InputMap` | trigger, type, color, filter | `PID` | — | Yes — `<map>` (`linkKey=TARGET`, `linkType=TYPE_MAP`) |
| `InputMapLink` | — | — | `COLLECTION_INPUT_MAP_LINKS` | No tag (`getXmlTag()` empty); body-only `Parent` aggregator |
| `Link` | own fieldsData (e.g. color, filter) | — | — | Yes — self-closing (`linkKey`, `linkType`, `linkFields`) |
| `DirectoryEntry` | — | `UID`, `PID`, `FILENAME` | `contents` | **No** — runtime only |

---

## 14. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `getCssClass()` | **Yes** (pure) | CSS class for the `BoxButton`. |
| `getXmlTag()` | **Yes** (pure) | XML element name. |
| `getCollectionHandler()` | **Yes** (pure) | Returns the handler this item registers into; `nullptr` for non-registering types. |
| `createPrettyName()` | Recommended | Human label. Default: `getPrimaryValue()`. |
| `createTooltip()` | Optional | Hover text. Default `""`. |
| `createUniqueId()` | Recommended | Stable collection key. Default hashes primary key via `getPrimaryValue()`. |
| `getPrimaryKey()` | When primary key ≠ `NAME` | Drives default `createUniqueId()` and `getPrimaryValue()`. |
| `xmlBody()` | When has serializable children | Inner XML content. Default `""` (self-closing). |
| `toXML()` | `DirNode`-backed `Parent` subclasses (`Input`, `Animation`, `Profile`) | Full file serialization via `XMLHelper::xmlHeader/Footer`. |
| `shouldSerialize(key, value)` | When fields need suppression | Return `false` to omit a field from XML. |
| `getValue()` / `setValue()` | `Link` only | Redirect `linkKey` lookups to target; silence writes to `linkKey`. |
