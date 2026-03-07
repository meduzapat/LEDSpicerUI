# LEDSpicerUI — Data System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.7 / data format 1.1.
> Some details (e.g. the `ignored` vector) are still under debate and marked accordingly.

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
9. [Visual and Textual Decoration](#9-visual-and-textual-decoration)
10. [Ignoring Fields During XML Serialization](#10-ignoring-fields-during-xml-serialization)
11. [Ownership — BoxButton and BoxButtonCollection](#11-ownership--boxbutton-and-boxbuttoncollection)
12. [Use Cases by Concrete Type](#12-use-cases-by-concrete-type)
13. [Quick Reference — Virtual Methods to Override](#13-quick-reference--virtual-methods-to-override)

---

## 1. Overview

`Data` is the central storage unit in LEDSpicerUI. Every piece of persistent configuration — a device, an element, an animation, an input — is an instance of a `Data` subclass.

Key design rules:

- `Data` only **holds** values. It does not render itself, open dialogs, or drive UI directly. All display logic lives in `DialogForm` and `BoxButton`.
- Persistent values (those that end up in XML) are stored in `fieldsData`.
- Runtime-only state that must never be serialized is stored in `properties`.
- A `Data*` is always **owned** by exactly one `BoxButton`. The `BoxButton` is owned by a `BoxButtonCollection`. The collection is either owned by a dialog or embedded inside a parent `Data` instance.

---

## 2. Class Hierarchy

```
Data
├── Link               — Wraps a pointer to another Data; acts as a reference.
├── DirNode            — Base for tree-navigable items (path-aware).
│   ├── DirectoryEntry — Runtime-only directory node. Never serialized.
│   └── FileData       — File-based items; stores FILENAME as a property.
│       ├── Input      — Input configuration. Owns sources & linkedMaps.
│       ├── Animation  — (similar pattern)
│       └── Profile    — (similar pattern)
├── Device             — Hardware device. Owns an Element collection.
├── Group              — Layout group. Owns an Element-reference collection.
├── Element            — Single LED element. May own strip children.
├── InputSource        — One hardware source inside an Input file. Owns maps.
├── InputMap           — Single trigger→action row inside an InputSource.
├── InputMapLink       — Cross-source link record.
└── ...                — Other domain types follow the same pattern.
```

`Selection` is a lightweight GTK widget (not a `Data` subclass) used by
`DialogSelect` to present clickable items. It holds a `Data*` but does not own
it. See [§7](#7-selection--selection).

---

## 3. Basic Data Storage — `fieldsData`

`fieldsData` is an `StringUMap` (`unordered_map<string, string>`). It is the
source of truth for everything that gets written to XML. All values are strings.

### Construction

A `Data` subclass always receives its initial map by move:

```cpp
// In the dialog's createData():
StringUMap raw {{"name", "MyDevice"}, {"port", "/dev/ttyUSB0"}};
return new Device(raw);   // raw is moved into fieldsData
```

The constructor signature is therefore always:

```cpp
explicit SomeData(StringUMap& data) : Data(data) { ... }
```

### Reading and writing values

```cpp
string port = myData->getValue("port");          // "" if missing
string port = myData->getValue("port", "auto");  // default if missing

myData->setValue("port", "/dev/ttyUSB1");
myData->unSet("port");                           // removes the key entirely
```

`getValues()` returns a `const StringUMap*` for read-only iteration.

### Unique identity

Every `Data` instance can produce a string key that uniquely identifies it
within its collection:

```cpp
const string id = myData->createUniqueId();
```

The default implementation hashes the primary-key field (usually `name`).
Subclasses override this when the identity requires more fields (e.g. `Device`
combines name + id + port).

### XML output

```cpp
const string xml = myData->toXML();
```

The base implementation emits all `fieldsData` key-value pairs as XML
attributes. Subclasses override to add child nodes or custom structure.

---

## 4. Runtime Properties — `properties`

`properties` is a second `StringUMap` that is **never** written to XML. Use it
for:

- Stable internal identifiers (`FILE_ID`, `FILENAME`, `fsId`).
- Flags computed at load time that must survive renames.
- Any cross-object context a `Data` needs but that has no place in the
  serialized config.

```cpp
myData->setProperty("fileId",  "file_3");
myData->setProperty("dirName", "shooters");

string id  = myData->getProperty("fileId");          // "file_3"
string dir = myData->getProperty("dirName", "root"); // default if missing
bool   has = myData->hasProperty("dirName");
myData->removeProperty("dirName");
```

### Rule of thumb

> If the value belongs in the XML config → `fieldsData`.
> If the value is purely for the UI's bookkeeping → `properties`.

---

## 5. Data That Stores Data (Composition)

A `Data` subclass can own child collections by embedding one or more
`BoxButtonCollection` members. The collection is typically wired to a child
`DialogForm` during `activate()`.

```cpp
// Device.hpp
class Device : public Data {
    ...
    Storage::BoxButtonCollection elements;  // owns all Element BoxButtons
};

// Device.cpp
void Device::activate() {
    DialogElement::getInstance()->setOwner(&elements, this);
}

const string Device::toXML() const {
    string r(createOpeningXML("device", fieldsData, ignored, false));
    for (const auto& e : elements)
        r += e->getData()->toXML();
    r += createClosingXML("device");
    return r;
}
```

The child `BoxButtonCollection` is destroyed with the parent `Data`. This
automatically `delete`s every `BoxButton` it holds, which in turn `delete`s
every child `Data`. Ownership is always strict and single.

### Directory nodes

`DirectoryEntry` follows the same pattern but represents a runtime filesystem
node. It owns a `BoxButtonCollection contents` of `FileData*` items. It is
never serialized — `load()` is a no-op override.

---

## 6. Links — `Link`

`Link` is a `Data` subclass that represents a **reference** to another `Data`
object. It does not copy the target's values; it holds a raw pointer and
delegates display and identity back to the target.

```cpp
struct LinkData {
    string       type;   // XML element name, e.g. "element"
    string       key;    // attribute name used to write the ID, e.g. "name"
    const Data*  link;   // the actual target Data object
};
```

### Creating a Link

```cpp
StringUMap emptyData;
auto* link = new Storage::Link(emptyData, {"element", "name", targetElement});
```

### How Link behaves

| Method | Behaviour |
|--------|-----------|
| `getCssClass()` | Delegates to the target. |
| `createPrettyName()` | Delegates to the target. |
| `createUniqueId()` | Delegates to the target. |
| `getValue(key)` | Returns target's value if key matches `linkInfo.key`; otherwise own `fieldsData`. |
| `toXML()` | Emits a self-closing XML element using `linkInfo.type` with the target's unique ID as the key attribute, plus any extra values in own `fieldsData`. |
| `operator==` | True if same object identity **or** if compared against the target pointer. |

### Typical use

Groups and profile sequences do not embed copies of their elements. They
contain `Link` objects pointing into the global `COLLECTION_ELEMENT` registry.
When an element is renamed or deleted, the `CollectionHandler` cascade removes
all `Link`s that referenced it.

---

## 7. Selection — `Selection`

`Selection` is a small GTK widget (`Gtk::FlowBoxChild` descendant) used
exclusively inside `DialogSelect`. It wraps a `Data*` **without owning it**
and makes it clickable in the "pick from available items" flow.

```
DialogSelect
 └─ boxAll (Gtk::FlowBox)
     └─ Selection*   ← holds Data* (not owned)
         └─ Data*    ← lives in a CollectionHandler
```

When the user confirms a selection, `DialogSelect` reads the `Data*` back out
of each selected `Selection`, creates a `Link` around it, and adds that `Link`
to the working `BoxButtonCollection`. Ownership then passes to the new
`BoxButton`.

`Selection` is never stored persistently. It is rebuilt from the collection
every time `DialogSelect` opens.

---

## 8. Activation and Deactivation

`activate()` and `deActivate()` are the lifecycle hooks that connect a `Data`
object to its child dialogs at runtime.

### `activate()`

Called by `DialogForm` when the user opens or expands a `Data` item for
editing. Its job is to call `setOwner()` on every child dialog, wiring the
dialog to this object's internal `BoxButtonCollection` and to `this` as the
owner context.

```cpp
void InputSource::activate() {
    // Wire the map dialog to this source's map collection.
    DialogInputMap::getInstance()->setOwner(&maps, this);
}
```

After `activate()` the child dialog knows:
- Where to put new items (`maps` collection).
- Which parent owns the dialog (`this`), for context-sensitive validation and
  XML scoping.

### `deActivate()`

Called on destruction (the base `~Data()` calls it) or whenever the item is
removed from a collection. Clean up any registrations in `CollectionHandler`
that were made during `activate()`.

```cpp
InputSource::~InputSource() {
    // Remove from the input-scoped source collection if still registered.
    string collectionId(COLLECTION_INPUT_SOURCES + getProperty(FILE_ID));
    if (CollectionHandler::getInstance(collectionId)->isSet(this))
        CollectionHandler::getInstance(collectionId)->remove(this);
}
```

### Rule

> `activate()` wires. `deActivate()` / destructor unwires.
> Never leave dangling registrations.

---

## 9. Visual and Textual Decoration

Each `Data` subclass controls how it appears in the UI through three virtual
methods. They are called **after** the data is fully populated (form realized).

### `getCssClass()`

Returns a CSS class string applied to the owning `BoxButton`. Use it to
visually distinguish item types or states.

```cpp
// Element.cpp
const string Element::getCssClass() const {
    return "ElementBoxButton";
}

// Input.cpp — state-aware styling
const string Input::getCssClass() const {
    return hasProperty("error") ? "InputBoxButton--error" : "InputBoxButton";
}
```

The returned class is applied directly to the `BoxButton` GTK widget; it maps
to a CSS rule in the application stylesheet.

### `createPrettyName()`

Returns the human-readable label shown on the `BoxButton`. The default reads
the field identified by `getPrimaryKey()` (usually `name`). Override when the
label should combine multiple fields or add runtime info.

```cpp
// Device.cpp — combines device type name and port
const string Device::createPrettyName() const {
    string name(fieldsData.at(NAME));
    string r(Defaults::devicesInfo.at(name).name);
    if (Defaults::isSerial(name))
        r += " Port: " + (fieldsData.at(PORT).empty() ? "<autodetect>" : fieldsData.at(PORT));
    return r;
}
```

### `createTooltip()`

Returns the tooltip shown on hover. The base returns an empty string (no
tooltip). Override to surface useful detail without cluttering the label.

```cpp
// InputSource.cpp
const string InputSource::createTooltip() const {
    return "Source " + createPrettyName() + " with "
        + std::to_string(maps.getSize()) + " maps";
}
```

### Updating the label after a data change

`BoxButton::updateLabel()` re-queries `createPrettyName()`, `createTooltip()`,
and `getCssClass()` and applies them. Call it after any `setValue()` that
affects the displayed text.

---

## 10. Ignoring Fields During XML Serialization

The `ignored` member (`StringUSet`, `mutable`) lists field keys that should be
omitted from XML output even though they exist in `fieldsData`.

> **Design note (under debate):** whether `ignored` stays as a per-instance
> mutable set or is replaced by a different mechanism is not yet decided.
> The current behaviour is described below; treat it as provisional.

### Populating `ignored`

Subclasses populate `ignored` in their constructor or inside `toXML()` based
on current values:

```cpp
// Element.cpp — suppress brightness when it equals the default "100"
const string Element::toXML() const {
    StringUSet ignored;
    if (fieldsData.at(BRIGHTNESS) == "100")
        ignored.insert(BRIGHTNESS);
    return createOpeningXML("element", fieldsData, ignored, true);
}
```

Note: `Element` creates a **local** `StringUSet` and passes it directly to
`createOpeningXML()`. This is the preferred pattern where the ignore logic is
conditional on value; it avoids mutating the member.

The member `Data::ignored` is used by the base `toXML()` and `valuesXML()`.
Subclasses that always want to suppress certain keys can insert into it at
construction time (as demonstrated in the unit-test fixture).

### The `FILENAME` special case

`FileData` strips `FILENAME` from `fieldsData` entirely during construction
and moves it to `properties`, so it never reaches `toXML()`. This is a cleaner
alternative to `ignored` for values that are truly never serialized.

---

## 11. Ownership — BoxButton and BoxButtonCollection

### The ownership chain

```
BoxButtonCollection
 └─ BoxButton          (heap, owned by collection)
     └─ Data*          (heap, owned by BoxButton)
```

- `BoxButton(Data* form)` takes ownership. Its destructor calls `delete data`.
- `BoxButtonCollection::create(Data*)` allocates a new `BoxButton` and appends
  it. Returns a reference to the new button.
- `BoxButtonCollection::remove(BoxButton&)` or `remove(Data*)` deletes the
  button (which deletes the `Data`) and erases it from the vector.
- `BoxButtonCollection::~BoxButtonCollection()` deletes every remaining button.

### Passing Data around

`Data*` is passed by raw pointer throughout the codebase. The pointer is valid
for as long as its `BoxButton` exists. **Never store a raw `Data*` beyond the
scope where you can guarantee the `BoxButton` is alive**, unless it is
registered in a `CollectionHandler` and you listen for cascade-delete signals.

### CollectionHandler — the global registry

`CollectionHandler` is a named singleton registry:

```cpp
CollectionHandler::getInstance("COLLECTION_ELEMENT")->add(myElement);
Data* found = CollectionHandler::getInstance("COLLECTION_ELEMENT")->get("myId");
```

It tracks live `Data*` pointers by their `createUniqueId()` string. When a
`Data` registers itself, it also registers `Dependency` structs so the handler
can cascade-delete all `Link`s and other dependants when the item is removed.

Subclasses typically register in `activate()` and unregister in their
destructor or `deActivate()`.

### Transferring between collections

`Data` is never moved between collections directly. Instead, the source
collection removes (and deletes) its `BoxButton`, and the destination dialog
creates a fresh `Data` from the same raw values via `createData(rawData)`. Use
`copyValues()` to get a `StringUMap` suitable for this.

---

## 12. Use Cases by Concrete Type

| Type | `fieldsData` | Properties | Child collections | Serialized |
|------|-------------|------------|-------------------|------------|
| `Device` | name, port, id, … | — | `elements` | Yes — `<device>` |
| `Element` | name, pin, position, brightness, … | `stripDescriptor` (when strip) | — | Yes — `<element>` |
| `Group` | name | — | Link→Element items | Yes — `<group>` |
| `Input` | name (input type) | `PATH`, `FILENAME` | `sources`, `linkedMaps` | Yes — per-file XML |
| `InputSource` | source (hardware id, may be empty) | `FILE_ID` | `maps` | Yes — `<maps>` |
| `InputMap` | trigger, type, target, … | — | — | Yes — `<map>` |
| `DirectoryEntry` | name (segment only) | — | `contents` (BoxButtonCollection of FileData*) | **No** — runtime only |
| `FileData` (base) | name (input/animation type) | `FILENAME`, `fsId` | — (overridden by subtypes) | Yes |
| `Link` | extra attributes (e.g. color) | — | — | Yes — self-closing element |

### Minimal concrete subclass skeleton

```cpp
// MyThing.hpp
class MyThing : public Data {
public:
    using Data::Data;
    ~MyThing();

    const string getCssClass()      const override;
    const string createPrettyName() const override;
    const string createUniqueId()   const override;
    const string toXML()            const override;
    void activate()                       override;
};

// MyThing.cpp
MyThing::~MyThing() {
    if (CollectionHandler::getInstance(COLLECTION_MY_THINGS)->isSet(this))
        CollectionHandler::getInstance(COLLECTION_MY_THINGS)->remove(this);
}

const string MyThing::getCssClass() const {
    return "MyThingBoxButton";
}

const string MyThing::createPrettyName() const {
    return getValue(NAME);
}

const string MyThing::createUniqueId() const {
    return Defaults::createCommonUniqueId({getValue(NAME)});
}

const string MyThing::toXML() const {
    return createOpeningXML("myThing", fieldsData, ignored, true);
}

void MyThing::activate() {
    // Wire child dialog if this type has children:
    // DialogMyChild::getInstance()->setOwner(&children, this);
    CollectionHandler::getInstance(COLLECTION_MY_THINGS)->add(this);
}
```

---

## 13. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `getCssClass()` | **Yes** (pure) | CSS class for the `BoxButton` widget. |
| `createPrettyName()` | Recommended | Human label; default uses primary key field. |
| `createTooltip()` | Optional | Hover text; default returns `""`. |
| `createUniqueId()` | Recommended | Stable collection key; default hashes primary key. |
| `toXML()` | Recommended | XML output; default emits flat attributes. |
| `activate()` | When has children | Wire child dialogs; register with `CollectionHandler`. |
| `deActivate()` | When `activate()` registers | Unregister from `CollectionHandler`. |
| `wipe()` | When has children | Clear child collections; base clears `fieldsData`. |
| `reset()` | When wipe must also unregister | Calls `wipe()` + removes from registry. |
| `getPrimaryKey()` | When primary key ≠ `name` | Drives `createPrettyName()` and default `createUniqueId()`. |
| `getValue()` | `Link` only | Redirect key lookups to the linked object. |