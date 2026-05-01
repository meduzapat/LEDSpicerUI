# LEDSpicerUI — Dialog System Developer Guide

> **Status:** Work in progress — reflects design as of v0.0.10 / data format 1.1.

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
    ├── DialogFileForm             — File-based dialogs.
    │   └── (DialogInput via DialogFormHost)
    ├── DialogProcess
    ├── DialogGroup
    ├── DialogProfile
    ├── DialogDirectory
    ├── DialogInputMap
    └── DialogInputLinkMaps

SingletonDialog<T>  — Mixin providing getInstance() / buildInstance().
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
| `items` | `BoxButtonCollection*` — where created items live. |
| `ownerData` | `const Data*` — parent context for secondary dialogs. |
| `currentData` | `Data*` — item being created, edited, or loaded. |
| `box` | `OrdenableFlowBox*` — display box in the UI. |
| `btnApply` | `Gtk::Button*` — confirm button. |
| `action` | `Actions` enum — `ADD`, `LOAD`, or `EDIT`. |
| `childDialogs` | `vector<DialogForm*>` — propagate refresh/reindex. |

Constructor responsibilities: retrieve all widget pointers, build child singletons, call `setSignalAdd()` and `setSignalApply()`, wire extra signals. **Never perform live data operations in constructors.**

The Add button almost always **lives in the parent dialog's layout**. The child dialog wires the signal to it during construction. The same button may be `get_widget`-ed by multiple constructors safely.

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
  → clearForm() → currentData = createData() → activate()
  → run()
  → [APPLY] isValid() → wipe() → storeData()
           → items->create() → add() → addButtons() → afterCreate()
  → deActivate() → hide()
```

### EDIT

```
onEditClicked()
  → clearForm() → currentData = boxButton->getData() → activate()
  → oldId = createUniqueId() → retrieveData() → run()
  → [APPLY]  wipe() → storeData() → replace(currentData, oldId) → updateLabel()
  → [CANCEL] deActivate() closes dialog; any type conversion already applied is final
  → hide()
```

### LOAD

```
createItems()
  → for each rawItem:
      createData(rawItem) → activate() → retrieveData() → isValid()
      → wipe() → storeData() → items->create() → add() → createSubItems()
      → deActivate()
```

---

## 7. Primary vs Secondary Dialogs

**Primary** — standalone, wired at init to a top-level collection:

```cpp
DialogDevice::getInstance()->setOwner(&devices);
```

**Secondary** — owned by a `Data`, wired in `activate()`:

```cpp
void MyData::activate() {
    DialogMyChild::getInstance()->setOwner(&children, this);
}
```

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
// In parent constructor:
DataDialogs::ChildDialog::buildInstance(builder, "ChildDialog");
childDialogs.push_back(ChildDialog::getInstance());
```

`refreshBox()` and `reindex()` propagate down the chain automatically.

> **Warning:** `show_all()` overrides any prior `hide()`. Always hide the **outermost** container box for a conditionally-visible section, or set `no-show-all` on the widget.

---

## 10. DialogFormHost — Type-Selector Dialogs

Intermediate base for dialogs driven by a primary type-selector combo.

**Use when:**
- A combo determines which child data/UI is shown.
- Changing the combo must convert or discard existing child data.
- A confirmation dialog is needed when switching an already-populated type.

| Member | Purpose |
|--------|---------|
| `previousName` | Guards against spurious `signal_changed` re-fires. |
| `handleTypeSwitch()` | Full decision tree for the type-selector combo. |
| `onConvert()` | Hook called before `onEmpty()` to migrate or discard children. |
| `markUsed()` | Updates a liststore's availability column via a predicate. |

### `handleTypeSwitch()` decision order

1. `name` empty → `onEmpty()` → return `false`.
2. `name == previousName` → return `false`.
3. `previousName` empty (first selection) → `previousName = name`, `onSelected()` → return `false`.
4. `name != previousName` and box non-empty → show dynamic confirmation; no → revert combo → return `false`.
5. `onConvert(previousName, name)` → `previousName = name` → `onEmpty()` → `onSelected()` → return `true`.

**Conversion is immediate and final.** Clicking Cancel after a type switch closes the dialog but does not undo the conversion — the data is already updated.

```cpp
selectorCombo->signal_changed().connect([this]() {
    string newName{selectorCombo->get_active_id()};
    string msg;
    // build dynamic warning from previousName / newName...
    if (handleTypeSwitch(box, msg))
        resetForm();
});
```

### `onConvert()` hook

Override in each subclass to migrate or discard children before the UI is cleared. Called with `(fromType, toType)`. Default is a no-op.

```cpp
void DialogDevice::onConvert(const string& fromType, const string& toType) noexcept {
    // compute newPins, then:
    DialogElement::getInstance()->handleLayoutChange(fromType, toType, newPins);
}
```

### `previousName` rule

Always set `previousName` **before** any `set_active_id()` that fires `signal_changed` — including inside `retrieveData()`:

```cpp
void MyDialog::retrieveData() {
    previousName = currentData->getValue(TYPE); // MUST come first
    selectorCombo->set_active_id(previousName);
}
```

---

## 11. DialogFileForm — File-Based Dialogs

Adds directory tracking for `FileData` subclasses:

```cpp
void setCurrentDirectory(DirectoryEntry* directory);
DirectoryEntry* getCurrentDirectory() const;
string getFullPath(const string& filename) const;
bool isUniqueFilename(const string& filename) const;
```

The navigator sets `currentDirectory` before opening. `isUniqueFilename()` scopes the check to the current directory only.

---

## 12. DialogSelect — Picking From Existing Items

Non-`DialogForm` dialog. Picks items from a pre-existing `CollectionHandler` and wraps them in `Link` objects. Configured via `SettingRequest` before opening. The selection list is rebuilt from the collection every open.

---

## 13. Creating a New Dialog — Step-by-Step

### 1 — Choose the base class

| Situation | Base |
|-----------|------|
| Type-selector combo that converts/discards child data | `DialogFormHost` |
| Manages files in a directory tree | `DialogFileForm` |
| Picks from existing items | `DialogSelect` |
| Everything else | `DialogForm` |

### 2 — Declare the class

```cpp
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
    Gtk::Entry* entryName = nullptr;
    DialogMyThing(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);
    string_view getType() const override;
    Storage::Data* createData(StringUMap& rawData) override;
};
```

### 3 — Implement the constructor

```cpp
DialogMyThing::DialogMyThing(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
    DialogForm(obj, builder)
{
    builder->get_widget_derived("BoxMyThings", box);
    builder->get_widget("BtnApplyMyThing", btnApply);
    Gtk::Button* btnAdd = nullptr;
    builder->get_widget("BtnAddMyThing",   btnAdd);
    builder->get_widget("EntryName",       entryName);
    setSignalAdd(btnAdd);
    setSignalApply();
}
```

### 4 — Register

**Primary:**

```cpp
DataDialogs::DialogMyThing::buildInstance(builder, "DialogMyThing");
DataDialogs::DialogMyThing::getInstance()->setOwner(&myThings);
```

**Secondary (in `MyData::activate()`):**

```cpp
DataDialogs::DialogMyThing::getInstance()->setOwner(&myThings, this);
```

---

## 14. Quick Reference — Virtual Methods to Override

| Method | Must override? | Purpose |
|--------|---------------|---------|
| `clearForm()` | **Yes** (pure) | Reset all widgets to blank state. |
| `isValid()` | **Yes** (pure) | Throw `Message` if invalid. |
| `storeData()` | **Yes** (pure) | Widgets → `currentData`. |
| `retrieveData()` | **Yes** (pure) | `currentData` → widgets. |
| `createUniqueId()` | **Yes** (pure) | Build unique ID from widget values. |
| `getType()` | **Yes** (pure) | Human-readable type name. |
| `createData(StringUMap&)` | **Yes** (pure) | Factory for the correct `Data` subclass. |
| `getCollectionHandler()` | **Yes** (pure) | Return this dialog's `CollectionHandler`. |
| `load(XMLHelper*)` | **Yes** (pure) | Dispatch raw XML into `createItems()`. |
| `resetForm()` | When extra state must be cleared | Calls `clearForm()` by default. |
| `addButtons(BoxButton&)` | When non-default buttons needed | Default adds Edit + Delete. |
| `createSubItems(XMLHelper*)` | When items have nested children | Called per item during `createItems`. |
| `afterCreate(BoxButton&)` | ADD-only hook | Called after ADD completes. |
| `afterDeleteConfirmation(BoxButton&)` | Pre-delete cleanup | Called before item is removed. |
| `setOwner(collection, owner)` | When extra wiring needed | Base stores `items` and `ownerData`. |

---

## 15. Common Pitfalls

### Signal re-fires during `retrieveData()`

Always set `previousName` before any `set_active_id()` in `retrieveData()`.

### `show_all()` overrides `hide()`

Always hide the **outermost** container box for a conditionally-visible section, or set `no-show-all` on the widget.

### Combo vs entry — resolve to one value

When a dialog has both a combo and a manual entry for the same logical value, merge them into a single resolver helper and drive all signal handlers through it to keep `createUniqueId()` consistent.
