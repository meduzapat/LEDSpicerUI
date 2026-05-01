# LEDSpicerUI — Dialog System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.13 / data format 1.1.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Class Hierarchy](#2-class-hierarchy)
3. [Singleton Pattern](#3-singleton-pattern)
4. [DialogForm — The Core Form Dialog](#4-dialogform--the-core-form-dialog)
5. [Stale vs Fresh Data](#5-stale-vs-fresh-data)
6. [The Form Lifecycle](#6-the-form-lifecycle)
7. [Primary vs Secondary Dialogs](#7-primary-vs-secondary-dialogs)
8. [BoxButton Decoration — Adding Action Buttons](#8-boxbutton-decoration--adding-action-buttons)
9. [Child Dialogs and Refresh Chains](#9-child-dialogs-and-refresh-chains)
10. [DialogFormHost — Type-Selector Dialogs](#10-dialogformhost--type-selector-dialogs)
11. [DialogFileForm — File-Based Dialogs](#11-dialogfileform--file-based-dialogs)
12. [DialogSelect — Picking From Existing Items](#12-dialogselect--picking-from-existing-items)
13. [Creating a New Dialog — Step-by-Step](#13-creating-a-new-dialog--step-by-step)
14. [Quick Reference — Virtual Methods to Override](#14-quick-reference--virtual-methods-to-override)
15. [Common Pitfalls](#15-common-pitfalls)

---

## 1. Overview

Dialogs are **stateless UI controllers**. Their only job is:
1. Present GTK widgets to the user.
2. Validate input via `isValid()`.
3. Move values between widgets and a `Data` object via `storeData()` / `retrieveData()`.
4. Create, edit, clone, or delete `Data` instances inside a `BoxButtonCollection`.

All dialogs are **singletons** for the application lifetime.

---

## 2. Class Hierarchy

```
Gtk::Dialog
├── GladeDialog<T>                — CRTP singleton base for non-form dialogs.
│   ├── DialogSelect
│   └── DialogColors
└── DialogForm                    — Base for all data-entry dialogs.
    ├── DialogFormHost             — Type-selector dialogs with conversion support.
    │   ├── DialogDevice
    │   ├── DialogRestrictor
    │   ├── DialogInput
    │   └── DialogInputSource
    ├── DialogProcess
    ├── DialogGroup
    ├── DialogProfile
    ├── DialogDirectory
    ├── DialogElement
    ├── DialogRestrictorMap
    ├── DialogInputMap
    └── DialogInputLinkMaps

SingletonDialog<T>  — Mixin providing getInstance() / buildInstance().
DirectoryAware      — Mixin adding current-directory tracking for file-based dialogs.
```

---

## 3. Singleton Pattern

```cpp
DialogDevice::buildInstance(builder, "DialogDevice");  // once at startup
DialogDevice* dlg = DialogDevice::getInstance();        // everywhere else
```

`buildInstance` is idempotent — safe to call from multiple constructors.

```cpp
class MyDialog : public DialogForm, public SingletonDialog<MyDialog> {
    friend class Gtk::Builder;
    ...
};
```

---

## 4. DialogForm — The Core Form Dialog

| Member | Purpose |
|--------|---------|
| `items` | `BoxButtonCollection*` — where created items live. Set by `setOwner()`. |
| `ownerData` | `Data*` — parent context for secondary dialogs. Set by `setOwner()`. |
| `currentData` | `Data*` — item currently being created, edited, or loaded. |
| `box` | `OrdenableFlowBox*` — display box in the UI. |
| `btnApply` | `Gtk::Button*` — confirm button. |
| `action` | `Actions` enum — `ADD`, `LOAD`, or `EDIT`. |
| `childDialogs` | `vector<DialogForm*>` — propagate refresh/reindex. |
| `familyToDialog` | Static map of collection family → child dialog. Populated via `registerChildDialog()`. |

Constructor responsibilities: retrieve all widget pointers, build child singletons via `registerChildDialog()`, call `setSignalAdd()` and `setSignalApply()`, wire extra signals. **Never perform live data operations in constructors.**

The Add button almost always **lives in the parent dialog's layout**. The child dialog wires the signal to it during construction. The same button may be `get_widget`-ed by multiple constructors safely.

### Key methods

| Method | Purpose |
|--------|---------|
| `setOwner(collection, owner)` | Points `items` and `ownerData` at the active parent; calls `refreshItems()`. |
| `removeOwner()` | Clears `items` and `ownerData`. |
| `wireChildrenDialogs()` | Calls `currentData->setUp()` then `setOwner()` on every registered child dialog. |
| `disconnectChildrenDialogs()` | Calls `removeOwner()` on every registered child dialog. |
| `registerChildDialog<T>(builder, id, family)` | Builds the child singleton and registers it in `familyToDialog`. |
| `getPrimaryChildCollection()` | Returns the first child `BoxButtonCollection*`, or `nullptr`. |
| `getChildCollection(family)` | Returns a specific child collection by family name. |

---

## 5. Stale vs Fresh Data

```
stale (currentData)  →  retrieveData()  →  widgets (fresh)
widgets (fresh)      →  storeData()     →  stale (currentData)
```

`isValid()` always reads from **widgets**. `ownerData` is also stale — read-only parent context.

---

## 6. The Form Lifecycle

### ADD

```
onAddClicked()
  → action = ADD → clearForm() → currentData = createData()
  → wireChildrenDialogs()
  → run()
  → [APPLY] currentData->wipe() → storeData() → markDirty()
            → items->create(currentData) → addButtons() → box->add() → afterCreate()
            → disconnectChildrenDialogs()
  → [CANCEL] disconnectChildrenDialogs() → delete currentData
  → currentData = nullptr → hide()
```

### EDIT

```
onEditClicked(boxButton)
  → action = EDIT → clearForm() → currentData = boxButton.getData()
  → wireChildrenDialogs() → retrieveData() → resetForm()
  → run()
  → [APPLY] markDirty() → oldId = createUniqueId()
            → currentData->wipe() → storeData()
            → syncRegistration(oldId) → boxButton.sync() → reindex()
  → disconnectChildrenDialogs() → currentData = nullptr → hide()
```

Note: any type conversion done during EDIT via the type-selector combo is **already committed** before APPLY/CANCEL. Cancel closes the dialog without undoing the conversion.

### LOAD

```
createItems(rawCollection, values)
  → for each rawItem:
      action = LOAD → clearForm() → currentData = createData(rawItem)
      → wireChildrenDialogs()
      → retrieveData() → isValid()          ← errors collected, item skipped on failure
      → currentData->wipe() → storeData()
      → items->create(currentData) → addButtons() → createSubItems(values)
      → disconnectChildrenDialogs()
  → currentData = nullptr
```

---

## 7. Primary vs Secondary Dialogs

**Primary** — standalone, wired at init to a top-level collection:

```cpp
DialogDevice::getInstance()->setOwner(&devices);
```

**Secondary** — registered in the parent dialog's constructor via `registerChildDialog()`. `wireChildrenDialogs()` calls `setOwner()` on them automatically when `currentData` is set:

```cpp
// In parent constructor:
registerChildDialog<DialogElement>(builder, "DialogElement", COLLECTION_ELEMENTS);

// wireChildrenDialogs() then does automatically:
DialogElement::getInstance()->setOwner(&currentData->children[COLLECTION_ELEMENTS], currentData);
```

`removeOwner()` / `disconnectChildrenDialogs()` tear down the wiring when the dialog closes.

---

## 8. BoxButton Decoration — Adding Action Buttons

Override `addButtons(BoxButton&)` to add custom buttons. Call `DialogForm::addButtons(bb)` at the end for standard Edit + Delete.

```cpp
void MyDialog::addButtons(Storage::BoxButton& bb) {
    // add custom button...
    DialogForm::addButtons(bb);
}
```

---

## 9. Child Dialogs and Refresh Chains

```cpp
// In parent constructor — builds singleton and registers in familyToDialog:
registerChildDialog<DialogElement>(builder, "DialogElement", COLLECTION_ELEMENTS);
```

`refreshBox()` and `reindex()` propagate down the `childDialogs` chain automatically.

> **Warning:** `show_all()` overrides any prior `hide()`. Always hide the **outermost** container box for a conditionally-visible section, or set `no-show-all` on the widget.

---

## 10. DialogFormHost — Type-Selector Dialogs

Intermediate base for dialogs driven by a primary type-selector combo. Adds type-switching with optional data conversion.

**Use when:**
- A combo determines which child data/UI is shown.
- Changing the combo must convert or discard existing child data.
- A dynamic confirmation is shown when switching an already-populated type.

| Member | Purpose |
|--------|---------|
| `previousName` | Guards against spurious `signal_changed` re-fires. |
| `selectorCombo` | The primary type-selector combo. |
| `listStore` | Backing model for `selectorCombo`. |
| `handleTypeSwitch(box, msg)` | Full decision tree for the type-selector combo. |
| `onConvert(fromType, toType)` | Override to migrate or discard children before UI clears. |
| `onEmpty()` | Pure virtual — clear all type-specific UI fields. |
| `onSelected()` | Pure virtual — prepare UI for the newly selected type. |
| `markUsed(predicate)` | Updates a liststore's availability column via a predicate. |
| `initializeSelector(emptyMsg, infoMap)` | Populates `selectorCombo` from a `Defaults::*Info` map. |

### `handleTypeSwitch()` decision order

1. Row index `-1` → return `false`.
2. `name` empty → `onEmpty()` → return `false`.
3. `previousName` empty (first selection) → `previousName = name`, `onSelected()` → return `ADD` only.
4. `previousName == name` → return `false`.
5. Data exists and box non-empty → show dynamic confirmation; no → revert combo → return `false`.
6. `onConvert(previousName, name)` → `previousName = name` → `onEmpty()` → `onSelected()` → return `true`.

**Conversion is immediate and final.** Cancel closes the dialog but does not undo any conversion — data is already updated.

```cpp
selectorCombo->signal_changed().connect([this]() {
    string newName{selectorCombo->get_active_id()};
    string msg;
    if (not newName.empty() and not previousName.empty()) {
        // build dynamic warning describing consequences...
        msg = "Are you sure you want to convert \"" + oldInfo.name + "\" into \"" + newInfo.name + "\"?";
        // append specific warnings (pin loss, profile loss, source collapse, etc.)
    }
    if (handleTypeSwitch(DialogChild::getInstance()->getBox(), msg))
        resetForm();
});
```

### `onConvert()` hook

Override in each `DialogFormHost` subclass to migrate or discard children before the UI is cleared. Called with `(fromType, toType)`. Default is a no-op.

| Dialog | `onConvert()` delegates to |
|--------|---------------------------|
| `DialogDevice` | `DialogElement::handleLayoutChange(fromType, toType, newPins)` |
| `DialogRestrictor` | `DialogRestrictorMap::trimToInterfaces(newInterfaces)` |
| `DialogInput` | `DialogInputSource::convertToSourceless()` or `convertToSourced()` + field cleanup |

### Conversion helpers

**`DialogElement::handleLayoutChange(fromType, toType, newPins)`**
1. Calls `changeNumberOfPins(newPins)` — removes elements whose pins exceed the new limit.
2. If `oldInfo.supportStrip && !newInfo.supportStrip` — removes all strip elements.
3. If `oldInfo.layoutRGB && !newInfo.layoutRGB` — calls `Element::splitRGB()` on surviving elements and clears positional fields (`POSITION`, `POSITIONS`, `COLORFORMAT`).

**`DialogRestrictorMap::trimToInterfaces(maxInterfaces)`**
- Removes all maps whose `RESTRICTOR_INTERFACE` value exceeds `maxInterfaces`.

**`DialogInputSource::convertToSourceless()`**
- Keeps the first source, converts it to the phantom (sets `SOURCELESS`, clears `SOURCE`), removes all other sources and their maps.

**`DialogInputSource::convertToSourced()`**
- Promotes the phantom source to a real source (removes `SOURCELESS` flag). `SOURCE` is left empty for the user to fill in.

### `previousName` rule

Always set `previousName` **before** any `set_active_id()` that fires `signal_changed` — including inside `retrieveData()`:

```cpp
void MyDialog::retrieveData() {
    previousName = currentData->getValue(NAME); // MUST come first
    selectorCombo->set_active_id(previousName);
}
```

---

## 11. DialogFileForm — File-Based Dialogs

`DirectoryAware` mixin adds current-directory tracking for dialogs that manage file-based `Data` objects (`Input`, `Animation`, `Profile`):

```cpp
void setCurrentDirectory(DirectoryEntry* directory);
DirectoryEntry* getCurrentDirectory() const;
```

The directory navigator sets the current directory before opening the dialog. `createUniqueId()` must include the directory path to scope uniqueness correctly.

---

## 12. DialogSelect — Picking From Existing Items

Non-`DialogForm` dialog. Picks items from a pre-existing `CollectionHandler` and wraps them in `Link` objects. Configured via `SelectionRequest` before opening. The selection list is rebuilt from the collection every open.

---

## 13. Creating a New Dialog — Step-by-Step

### 1 — Choose the base class

| Situation | Base |
|-----------|------|
| Type-selector combo that converts/discards child data | `DialogFormHost` |
| Manages files in a directory tree | `DialogForm` + `DirectoryAware` |
| Picks from existing items | `DialogSelect` |
| Everything else | `DialogForm` |

### 2 — Declare the class

```cpp
class DialogMyThing : public DialogForm, public SingletonDialog<DialogMyThing> {
    friend class Gtk::Builder;
public:
    virtual ~DialogMyThing() = default;
    void load(XMLHelper* values)  noexcept override;
    void clearForm()              noexcept override;
    void isValid()          const          override;
    void storeData()              noexcept override;
    void retrieveData()           noexcept override;
    string createUniqueId() const noexcept override;
protected:
    Gtk::Entry* entryName = nullptr;
    DialogMyThing(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;
    const string& getType()                   const noexcept override;
    Storage::Data* createData(StringUMap& rawData) const noexcept override;
};
```

### 3 — Implement the constructor

```cpp
DialogMyThing::DialogMyThing(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
    DialogForm(obj, builder)
{
    // Register child dialogs first.
    registerChildDialog<DialogMyChild>(builder, "DialogMyChild", COLLECTION_MY_CHILDREN);

    builder->get_widget_derived("BoxMyThings", box);
    builder->get_widget("BtnApplyMyThing", btnApply);
    Gtk::Button* btnAdd = nullptr;
    builder->get_widget("BtnAddMyThing", btnAdd);
    builder->get_widget("EntryName",     entryName);
    setSignalAdd(btnAdd);
    setSignalApply();
}
```

### 4 — Register as primary dialog

```cpp
DataDialogs::DialogMyThing::buildInstance(builder, "DialogMyThing");
DataDialogs::DialogMyThing::getInstance()->setOwner(&myThings);
```

Secondary dialogs self-register via `registerChildDialog()` — no manual `setOwner()` needed for them.

---

## 14. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `clearForm()` | **Yes** (pure) | Reset all widgets to blank state. |
| `isValid()` | **Yes** (pure) | Throw `Message` if invalid. |
| `storeData()` | **Yes** (pure) | Widgets → `currentData`. |
| `retrieveData()` | **Yes** (pure) | `currentData` → widgets. |
| `createUniqueId()` | **Yes** (pure) | Build unique ID from widget values. |
| `getType()` | **Yes** (pure) | Human-readable type name string ref. |
| `createData(StringUMap&)` | **Yes** (pure) | Factory for the correct `Data` subclass. |
| `load(XMLHelper*)` | **Yes** (pure) | Dispatch raw XML into `createItems()`. |
| `resetForm()` | When extra state must be set after type switch | Calls `refreshItems()` by default. |
| `addButtons(BoxButton&)` | When non-default buttons needed | Default adds Edit + Delete. |
| `createSubItems(XMLHelper*)` | When items have nested children | Called per item during `createItems()`. |
| `afterCreate(BoxButton&)` | ADD-only hook | Called after ADD completes and button is in the box. |
| `afterDeleteConfirmation(BoxButton&)` | Pre-delete cleanup | Called before item is removed. |
| `setOwner(collection, owner)` | When extra wiring needed on activation | Base stores `items` and `ownerData`. |
| `removeOwner()` | When extra teardown needed | Base clears `items` and `ownerData`. |
| `wireChildrenDialogs()` | Rarely — only for non-`Parent` data | Base handles `Parent` children automatically. |
| `onConvert(fromType, toType)` | `DialogFormHost` subclasses with children | Migrate or discard children before `onEmpty()`. |
| `onEmpty()` | `DialogFormHost` subclasses (pure) | Clear type-specific UI fields. |
| `onSelected()` | `DialogFormHost` subclasses (pure) | Prepare UI for newly selected type. |

---

## 15. Common Pitfalls

### `previousName` must be set before `set_active_id()`

In `retrieveData()`, always assign `previousName` before calling `selectorCombo->set_active_id()`, otherwise the `signal_changed` fires and `handleTypeSwitch()` misreads the state.

### `show_all()` overrides `hide()`

Always hide the **outermost** container box for a conditionally-visible section, or set `no-show-all` on the widget in the Glade file.

### Combo vs entry — resolve to one value

When a dialog has both a combo and a manual entry for the same logical value (e.g. `DialogInputSource`), merge them into a single `resolvedSource()` helper and drive all signal handlers and `createUniqueId()` through it.

### Type conversion is final

`onConvert()` runs before `onEmpty()` and commits changes to the data immediately. There is no undo — Cancel closes the dialog but leaves the converted data in place. Always show a dynamic warning message describing the specific consequences before confirming.
