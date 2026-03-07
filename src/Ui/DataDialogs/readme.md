# LEDSpicerUI — Dialog System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.7 / data format 1.1.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Class Hierarchy](#2-class-hierarchy)
3. [Singleton Pattern — SingletonDialog / GladeDialog](#3-singleton-pattern--singletondialog--gladedialog)
4. [DialogForm — The Core Form Dialog](#4-dialogform--the-core-form-dialog)
5. [Stale vs Fresh Data](#5-stale-vs-fresh-data)
6. [The Form Lifecycle](#6-the-form-lifecycle)
7. [Primary vs Secondary Dialogs](#7-primary-vs-secondary-dialogs)
8. [BoxButton Decoration — Adding Action Buttons](#8-boxbutton-decoration--adding-action-buttons)
9. [Child Dialogs and Refresh Chains](#9-child-dialogs-and-refresh-chains)
10. [DialogFileForm — File-Based Dialogs](#10-dialogfileform--file-based-dialogs)
11. [DialogSelect — Picking From Existing Items](#11-dialogselect--picking-from-existing-items)
12. [Creating a New Dialog — Step-by-Step](#12-creating-a-new-dialog--step-by-step)
13. [Quick Reference — Virtual Methods to Override](#13-quick-reference--virtual-methods-to-override)

---

## 1. Overview

Dialogs in LEDSpicerUI are **stateless UI controllers**. They hold no data
themselves. Their only job is:

1. Present GTK widgets to the user.
2. Validate input via `isValid()`.
3. Move values between GTK widgets and a `Data` object via `storeData()` /
   `retrieveData()`.
4. Create, edit, clone, or delete `Data` instances inside a
   `BoxButtonCollection`.

All dialogs are **singletons** — there is exactly one instance of each dialog
class for the lifetime of the application. They are initialized once from the
GTK Builder and reused for every open/edit/delete cycle.

---

## 2. Class Hierarchy

```
Gtk::Dialog
├── GladeDialog<T>                — CRTP singleton base for non-form dialogs.
│   └── DialogSelect              — Pick-from-collection dialog.
└── DialogForm (: Gtk::Dialog)    — Base for all data-entry dialogs.
    ├── DialogDevice
    ├── DialogRestrictor
    ├── DialogProcess
    ├── DialogGroup
    ├── DialogProfile
    ├── DialogDirectory
    ├── DialogFileForm             — Intermediate base for file-based dialogs.
    │   ├── DialogInput
    │   ├── DialogAnimation        — (similar to DialogInput)
    │   └── DialogProfile          — (file-based profile variant)
    ├── DialogInputSource
    ├── DialogInputMap
    └── DialogInputLinkMaps

SingletonDialog<T>                — Mixin providing getInstance() / buildInstance().
```

`DialogForm` subclasses mix in `SingletonDialog<T>` to gain the singleton
machinery. `DialogSelect` uses `GladeDialog<T>` which provides the same
singleton interface but a simpler base.

---

## 3. Singleton Pattern — SingletonDialog / GladeDialog

Every dialog is accessed through two static methods:

```cpp
// First call at app startup — constructs the instance from the GTK Builder.
DialogDevice::buildInstance(builder, "DialogDevice");

// Any subsequent call — returns the already-constructed pointer.
DialogDevice* dlg = DialogDevice::getInstance();
```

`buildInstance` calls `builder->get_widget_derived(widgetId, instance)`, which
constructs the C++ object on first call and returns the cached pointer on every
subsequent call. The widget must exist in the `.glade` file under the given ID.

### Adding the mixin to a new dialog

```cpp
class MyDialog : public DialogForm, public SingletonDialog<MyDialog> {
    friend class Gtk::Builder; // Required — builder calls a protected constructor.
    ...
};
```

The `friend class Gtk::Builder` declaration is mandatory; without it the builder
cannot reach the protected constructor.

---

## 4. DialogForm — The Core Form Dialog

`DialogForm` is the abstract base for every data-entry dialog. It manages:

| Member | Purpose |
|--------|---------|
| `items` | `BoxButtonCollection*` — where created/edited items live. Set by `setOwner()`. |
| `ownerData` | `const Data*` — parent context for secondary dialogs. |
| `currentData` | `Data*` — the item currently being created, edited, or loaded. |
| `box` | `OrdenableFlowBox*` — the display box in the UI that shows all items. |
| `btnApply` | `Gtk::Button*` — the confirm button wired by `setSignalApply()`. |
| `action` | `Actions` enum — `ADD`, `LOAD`, or `EDIT`. |
| `childDialogs` | `vector<DialogForm*>` — child dialogs refreshed when this one refreshes. |

### Constructor responsibilities

Inside the constructor (called by the builder), a dialog must:

1. Retrieve all widget pointers from the builder with `builder->get_widget(...)`.
2. Call `builder->get_widget_derived(...)` for `OrdenableFlowBox` and any derived
   child widget types.
3. Build any child dialog singletons that haven't been built yet.
4. Call `setSignalAdd(btnAdd)` and `setSignalApply()` to wire the standard buttons.
5. Add any child `DialogForm*` to `childDialogs`.
6. Wire any extra signals (combo changes, sensitivity toggles, etc.).

The builder lifetime is **not** assumed to persist after construction, so all
pointers must be retrieved inside the constructor.

---

## 5. Stale vs Fresh Data

A core concept throughout the dialog system is the distinction between **stale**
and **fresh** data.

**`currentData` holds stale data** — the last committed, validated state of the
item. During EDIT it is the saved version. During ADD it is a freshly allocated,
empty `Data` object — stale in the sense that it has not been touched by the
user yet, but there is no previous state to restore either.

**The form widgets hold fresh data** — whatever the user has typed or selected
right now, the live uncommitted state.

The cycle is always:

```
stale (currentData)  →  retrieveData()  →  widgets (fresh)
widgets (fresh)      →  storeData()     →  stale (currentData)
```

`isValid()` always reads from the **widgets** (fresh), never from `currentData`.
`storeData()` and `retrieveData()` are the only legal transfer points between
the two states.

**`ownerData` also holds stale data** — the last saved state of the parent
`Data` that activated this secondary dialog. It is read-only (`const Data*`)
because the child dialog has no business modifying the parent's committed state.
Use it for context-sensitive validation, scoping uniqueness checks, or
populating combos that depend on parent fields.

```cpp
// Scope a uniqueness check to the parent input file.
void DialogInputSource::isValid() const {
    string collectionId(COLLECTION_INPUT_SOURCES + ownerData->getProperty(FILE_ID));
    if (action != Actions::EDIT and CollectionHandler::getInstance(collectionId)->isIdSet(createUniqueId()))
        throw Message("This source already exists in this input file.");
}
```

---

## 6. The Form Lifecycle

### ADD flow

```
User clicks Add button
    → onAddClicked()
        → action = ADD
        → resetForm() / clearForm()
        → currentData = createData()       ← empty Data object
        → currentData->activate()          ← wires child dialogs
        → run()                            ← GTK modal loop
            [user fills form and clicks Apply]
            → isValid()                    ← throws Message on error
            → response(RESPONSE_APPLY)
        → currentData->wipe()
        → storeData()                      ← writes widgets → currentData
        → items->create(currentData)       ← BoxButton takes ownership
        → getCollectionHandler()->add()
        → addButtons(boxButton)
        → afterCreate(boxButton)           ← optional hook
        → currentData->deActivate()
        → hide()
```

### EDIT flow

```
User clicks Edit button on a BoxButton
    → onEditClicked(boxButton)
        → action = EDIT
        → resetForm() / clearForm()
        → currentData = boxButton->getData()
        → currentData->activate()
        → oldId = currentData->createUniqueId()
        → retrieveData()                   ← writes currentData → widgets
        → run()
            [user edits and clicks Save]
            → isValid()
            → response(RESPONSE_APPLY)
        → currentData->wipe()
        → storeData()                      ← writes widgets → currentData
        → getCollectionHandler()->replace(currentData, oldId)
        → boxButton->updateLabel()
        → childDialogs reindex
        → currentData->deActivate()
        → hide()
```

### LOAD flow (reading from XML)

```
DialogForm::createItems(rawCollection, values)
    for each rawItem:
        → action = LOAD
        → resetForm()
        → currentData = createData(rawItem) ← pre-populated Data
        → currentData->activate()
        → retrieveData()                    ← round-trips through the form
        → isValid()
        → currentData->wipe()
        → storeData()                       ← re-stores sanitized values
        → items->create(currentData)
        → getCollectionHandler()->add()
        → addButtons(boxButton)
        → createSubItems(values)            ← loads nested data
        → currentData->deActivate()
```

The LOAD round-trip through `retrieveData` → `isValid` → `storeData` is
intentional: it sanitizes and normalizes raw XML data through the same
validation path as user input.

### DELETE flow

```
User clicks Delete button
    → (optional confirmation dialog)
    → onDelClicked(boxButton)
        → currentData = boxButton->getData()
        → currentData->activate()
        → afterDeleteConfirmation(boxButton) ← optional hook
        → getCollectionHandler()->remove(currentData)
        → box->remove(boxButton)
        → items->remove(boxButton)           ← deletes BoxButton → deletes Data
        → currentData = nullptr
```

---

## 7. Primary vs Secondary Dialogs

### Primary dialogs

Primary dialogs are wired at startup to a **top-level** `BoxButtonCollection`
owned by `MainWindow`. They have no `ownerData`.

```cpp
// In MainWindow constructor:
DialogDevice::getInstance()->setOwner(&devices);     // no owner
DialogGroup::getInstance()->setOwner(&groups);
```

### Secondary dialogs

Secondary dialogs are owned by a `Data` object and are wired at runtime inside
that object's `activate()`. They gain both a `BoxButtonCollection` and an
`ownerData` pointer.

```cpp
// In Device::activate():
DialogElement::getInstance()->setOwner(&elements, this);

// In InputSource::activate():
DialogInputMap::getInstance()->setOwner(&maps, this);
```

`ownerData` is available inside the dialog as a `const Data*` for
context-sensitive validation (e.g. checking how many sources the parent already
has, or scoping uniqueness checks to the parent file).

### Rule

> If the dialog can be opened without any parent item being active → primary.
> If the dialog only makes sense in the context of a specific `Data` instance → secondary.

---

## 8. BoxButton Decoration — Adding Action Buttons

After a `BoxButton` is created, `addButtons()` is called to attach action
buttons to it. The base implementation adds Edit + Delete:

```cpp
void DialogForm::addButtons(Storage::BoxButton& boxButton) {
    createEditButton(boxButton);
    createDeleteButton(boxButton);
    boxButton.show_all();
}
```

Override `addButtons()` when a type needs different or additional buttons (e.g.
Clone, or a type-specific open button):

```cpp
void DialogInput::addButtons(Storage::BoxButton& boxButton) {
    createOpenButton(boxButton);   // custom: opens the file in the navigator
    createEditButton(boxButton);
    createDeleteButton(boxButton);
    boxButton.show_all();
}
```

Available helpers:

| Helper | What it creates |
|--------|----------------|
| `createEditButton(bb)` | Opens `onEditClicked`. |
| `createDeleteButton(bb, askConfirmation)` | Calls `onDelClicked`; confirms by default. |
| `createCloneButton(bb)` | Calls `onCloneClicked`; copies values, finds a unique name. |

Buttons are `Gtk::make_managed` — GTK owns their memory once packed into the
`BoxButton` widget.

### Post-create hook

`afterCreate(BoxButton&)` is called once immediately after a new item is added
during the ADD flow. Use it for any one-time setup that should not run during
EDIT or LOAD (e.g. opening a child dialog automatically after creation).

---

## 9. Child Dialogs, Builder Instantiation, and Refresh Chains

### Instantiation inside child constructors

If a dialog needs a sibling or child dialog to be available while it is being
constructed (e.g. to wire signals to it), that dialog must be instantiated
**from the builder inside the constructor that needs it**, not assumed to exist
already.

```cpp
// DialogInputSource.cpp — builds DialogInputMap before wiring to it.
DialogInputSource::DialogInputSource(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
    DialogForm(obj, builder)
{
    // DialogInputMap does not exist yet — build it here.
    DataDialogs::DialogInputMap::buildInstance(builder, "DialogInputMap");

    builder->get_widget_derived("BoxInputSources", box);
    // ... wire signals that reference DialogInputMap::getInstance() ...
}
```

`buildInstance` is idempotent: if the instance was already created by an outer
constructor, the call is a no-op and the cached pointer is returned. Order of
construction therefore does not need to be globally coordinated; each dialog
self-declares its dependencies.

The builder reference passed to the constructor is guaranteed valid for the
duration of that constructor call — which is all that is needed.

### The Add button — lives outside, handled inside

The Add button that opens a dialog is almost always **a widget that lives in
the parent dialog's layout**, not inside the child dialog's own window. The
child dialog merely connects a signal to it during construction.

```cpp
// DialogInput.cpp — BtnAddInput lives in the Input dialog layout, handled here.
builder->get_widget("BtnAddInput", btnAddInput);
setSignalAdd(btnAddInput);

// DialogInputSource.cpp — BtnAddInputSource also lives in the Input dialog layout.
builder->get_widget("BtnAddInputSource", btnAdd);
setSignalAdd(btnAdd);
```

Some parent dialogs retrieve the same button pointer themselves — for UI
purposes only (show/hide, sensitivity changes) — without wiring the open
action. The actual "open this dialog" signal is always owned by the dialog
that handles it:

```cpp
// DialogInput.cpp — retrieved here for visibility control only:
builder->get_widget("BtnAddInputSource", btnAddInputSource);
// no setSignalAdd() call — DialogInputSource owns that signal.
```

This means the same button may be `get_widget`-ed by multiple constructors.
That is safe: the builder cache returns the same underlying GTK object each
time.

### Refresh chain propagation

```cpp
// In DialogInput constructor:
DataDialogs::DialogInputSource::buildInstance(builder, "DialogInputSource");
childDialogs.push_back(DialogInputSource::getInstance());
```

`refreshBox()` and `reindex()` propagate down the chain automatically:

```cpp
void DialogForm::refreshBox() {
    for (auto childDialog : childDialogs) childDialog->refreshBox();
    box->wipe();
    if (not items) return;
    items->populateBox(box);
    box->show_all();
}
```

This means a parent dialog repopulating its display box also causes all child
display boxes to repopulate — important when navigating away and back.

---

## 10. DialogFileForm — File-Based Dialogs

`DialogFileForm` is an intermediate base for dialogs that manage `FileData`
subclasses (`Input`, `Animation`, `Profile`). It adds directory tracking:

```cpp
void setCurrentDirectory(Storage::DirectoryEntry* directory);
Storage::DirectoryEntry* getCurrentDirectory() const;
string getFullPath(const string& filename) const;
bool isUniqueFilename(const string& filename) const;
```

The navigator sets `currentDirectory` before opening the dialog, so the dialog
always knows which directory a new file should land in. `isUniqueFilename()`
scopes the uniqueness check to that directory only — the same filename is valid
in a sibling directory.

```cpp
// In a DialogFileForm::isValid():
if (not isUniqueFilename(entryFileName->get_text()))
    throw Message("A file with that name already exists in this directory.");
```

---

## 11. DialogSelect — Picking From Existing Items

`DialogSelect` is a specialized, non-`DialogForm` dialog used when items need
to be **chosen from a pre-existing collection** rather than created from
scratch. It creates `Link` objects pointing to the chosen `Data` items.

### SettingRequest

Before opening, the caller configures `DialogSelect` via a `SettingRequest`
struct:

```cpp
const DialogSelect::SettingRequest mySetting {
    workingBox,           // OrdenableFlowBox*& where selected Links will appear
    NAME,                 // attribute key used to serialize the link (e.g. "name")
    TYPE_ELEMENT,         // XML element name for the resulting Link (e.g. "element")
    COLLECTION_ELEMENT,   // CollectionHandler name to source items from
    DialogSelect::BUTTON_COLORER | DialogSelect::BUTTON_DELETER  // buttons on each result
};
```

The `SettingRequest` is typically a `const` member of the calling dialog,
declared inline:

```cpp
// In DialogGroup.hpp:
const DialogSelect::SettingRequest groupElementsSetting {
    boxElements,
    NAME,
    TYPE_ELEMENT,
    COLLECTION_ELEMENT,
    DialogSelect::BUTTON_DELETER,
};
```

### Opening the dialog

```cpp
// Wire destinations once (usually in the calling dialog's constructor):
DialogSelect::getInstance()->setDestinations(
    {{TYPE_ELEMENT, &myElementLinks}},  // map: type → BoxButtonCollection*
    this                                 // caller Data* for context
);

// On button click:
DialogSelect::getInstance()->setSettings(groupElementsSetting);
DialogSelect::getInstance()->runSelection();
```

`runSelection()` shows the dialog, populates all available items from the
source collection as `Selection` widgets, marks already-chosen ones as
selected, and waits. On confirm it wipes the working collection, then creates
a fresh `Link` for each selected item.

### Button flags

| Flag | Effect |
|------|--------|
| `BUTTON_DELETER` | Adds a delete button to each selected item's `BoxButton`. |
| `BUTTON_COLORER` | Adds a color-picker button (for elements/groups in profiles). |
| `BUTTON_EDITER` | Adds an edit button that re-opens `DialogSelect` for that item. |

Flags can be combined with `|`.

---

## 12. Creating a New Dialog — Step-by-Step

### 1 — Add the widget to the `.glade` file

Create a `GtkDialog` with the desired ID (e.g. `DialogMyThing`) containing all
the form fields plus an Apply button with `response-id = -10` (GTK APPLY).

### 2 — Create the header

```cpp
// DialogMyThing.hpp
#include "DialogForm.hpp"
#include "Storage/MyThing.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

class DialogMyThing : public DialogForm, public SingletonDialog<DialogMyThing> {

    friend class Gtk::Builder;

public:

    virtual ~DialogMyThing() = default;

    void load(XMLHelper* values) override;
    Storage::CollectionHandler* getCollectionHandler() const override;
    void clearForm() override;
    void isValid() const override;
    void storeData() override;
    void retrieveData() override;
    const string createUniqueId() const override;

protected:

    Gtk::Entry* entryMyThingName = nullptr;

    DialogMyThing(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

    const string getType() const override;
    Storage::Data* createData(StringUMap& rawData) override;
};

} // namespace
```

### 3 — Implement the source file

```cpp
// DialogMyThing.cpp
#include "DialogMyThing.hpp"
using namespace LEDSpicerUI::Ui::DataDialogs;

DialogMyThing::DialogMyThing(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
    DialogForm(obj, builder)
{
    Gtk::Button* btnAdd = nullptr;
    builder->get_widget_derived("BoxMyThings", box);
    builder->get_widget("BtnAddMyThing",       btnAdd);
    builder->get_widget("BtnApplyMyThing",     btnApply);
    builder->get_widget("EntryMyThingName",    entryMyThingName);

    setSignalAdd(btnAdd);
    setSignalApply();
}

const string DialogMyThing::getType() const { return "My Thing"; }

Storage::Data* DialogMyThing::createData(StringUMap& rawData) {
    return new Storage::MyThing(rawData);
}

Storage::CollectionHandler* DialogMyThing::getCollectionHandler() const {
    return CollectionHandler::getInstance(COLLECTION_MY_THINGS);
}

void DialogMyThing::clearForm() {
    entryMyThingName->set_text("");
}

void DialogMyThing::isValid() const {
    const string name(entryMyThingName->get_text());
    if (name.empty())
        throw Message("Name cannot be empty.");
    if (action != Actions::EDIT and getCollectionHandler()->isIdSet(createUniqueId()))
        throw Message("A thing named '" + name + "' already exists.");
}

void DialogMyThing::storeData() {
    currentData->setValue(NAME, entryMyThingName->get_text());
}

void DialogMyThing::retrieveData() {
    entryMyThingName->set_text(currentData->getValue(NAME));
}

const string DialogMyThing::createUniqueId() const {
    return Defaults::createCommonUniqueId({entryMyThingName->get_text()});
}

void DialogMyThing::load(XMLHelper* values) {
    StringUMapVector& raw = values->getData(COLLECTION_MY_THINGS);
    createItems(raw, values);
}
```

### 4 — Register the dialog

**Primary** (top-level collection, in `MainWindow`):

```cpp
// MainWindow.cpp constructor:
DataDialogs::DialogMyThing::buildInstance(builder, "DialogMyThing");
DataDialogs::DialogMyThing::getInstance()->setOwner(&myThings);
```

**Secondary** (owned by a `Data` object, in `MyParent::activate()`):

```cpp
void MyParent::activate() {
    DataDialogs::DialogMyThing::getInstance()->setOwner(&myThings, this);
}
```

---

## 13. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `clearForm()` | **Yes** (pure) | Reset all GTK widgets to a blank state. |
| `isValid()` | **Yes** (pure) | Throw `Message` if the current widget values are invalid. |
| `storeData()` | **Yes** (pure) | Write widget values into `currentData`. |
| `retrieveData()` | **Yes** (pure) | Write `currentData` values into widgets. |
| `createUniqueId()` | **Yes** (pure) | Build a unique ID from the current widget values (not from `currentData`). |
| `getType()` | **Yes** (pure) | Human-readable type name used in dialog titles and error messages. |
| `createData(StringUMap&)` | **Yes** (pure) | Factory: construct the correct `Data` subclass from a raw map. |
| `getCollectionHandler()` | **Yes** (pure) | Return the `CollectionHandler` for this dialog's items. |
| `load(XMLHelper*)` | **Yes** (pure) | Dispatch raw XML data into `createItems()`. |
| `resetForm()` | When extra state must be cleared beyond widgets | Calls `clearForm()` by default; add any extra resets here. |
| `addButtons(BoxButton&)` | When non-default buttons are needed | Default adds Edit + Delete. |
| `createSubItems(XMLHelper*)` | When items have nested children to load | Called per item during `createItems`. |
| `afterCreate(BoxButton&)` | When something should happen only on first-time creation | Hook after ADD completes. |
| `afterDeleteConfirmation(BoxButton&)` | When a deletion needs extra cleanup | Called before the item is removed. |
| `setOwner(collection, owner)` | When extra wiring is needed beyond base | Base stores `items` and `ownerData`; call `DialogForm::setOwner` first. |