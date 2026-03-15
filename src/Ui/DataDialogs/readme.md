# LEDSpicerUI — Dialog System Developer Guide
 
> **Status:** Work in progress — reflects design as of v0.0.9 / data format 1.1.
 
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
10. [DialogFormHost — Type-Selector Dialogs](#10-dialogformhost--type-selector-dialogs)
11. [DialogFileForm — File-Based Dialogs](#11-dialogfileform--file-based-dialogs)
12. [DialogSelect — Picking From Existing Items](#12-dialogselect--picking-from-existing-items)
13. [Creating a New Dialog — Step-by-Step](#13-creating-a-new-dialog--step-by-step)
14. [Quick Reference — Virtual Methods to Override](#14-quick-reference--virtual-methods-to-override)
15. [Common Pitfalls](#15-common-pitfalls)
 
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
    ├── DialogFormHost             — Intermediate base for type-selector dialogs.
    │   ├── DialogDevice
    │   ├── DialogRestrictor
    │   ├── DialogInput            — Also inherits DialogFileForm.
    │   └── DialogInputSource
    ├── DialogFileForm             — Intermediate base for file-based dialogs.
    │   └── (DialogInput via DialogFormHost)
    ├── DialogProcess
    ├── DialogGroup
    ├── DialogProfile
    ├── DialogDirectory
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
empty `Data` object.
 
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
 
**`ownerData` also holds stale data** — read-only (`const Data*`) parent context.
Use it for scoping uniqueness checks or populating combos that depend on parent
fields.
 
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
            → isValid()
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
context-sensitive validation or XML scoping.
 
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
 
Override `addButtons()` when a type needs different or additional buttons.
 
Available helpers:
 
| Helper | What it creates |
|--------|----------------|
| `createEditButton(bb)` | Opens `onEditClicked`. |
| `createDeleteButton(bb, askConfirmation)` | Calls `onDelClicked`; confirms by default. |
| `createCloneButton(bb)` | Calls `onCloneClicked`; copies values, finds a unique name. |
 
---
 
## 9. Child Dialogs and Refresh Chains
 
### Instantiation inside child constructors
 
If a dialog needs a sibling or child dialog while it is being constructed, that
dialog must be instantiated **from the builder inside the constructor that needs
it**. `buildInstance` is idempotent — safe to call from multiple constructors.
 
### The Add button — lives outside, handled inside
 
The Add button that opens a dialog almost always **lives in the parent dialog's
layout**, not inside the child dialog's own window. The child dialog wires the
signal to it during construction.
 
```cpp
// DialogInput.cpp — retrieved here for visibility control only:
builder->get_widget("BtnAddInputSource", btnAddInputSource);
// no setSignalAdd() call — DialogInputSource owns that signal.
```
 
The same button may be `get_widget`-ed by multiple constructors safely.
 
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
 
> **Warning:** `show_all()` propagates through all widget descendants and will
> override any `hide()` call made earlier. Conditionally-visible sections that
> must survive `show_all()` must either have `no-show-all` set in the Glade
> file, or be controlled by hiding their outermost container box rather than an
> inner widget. Always hide the outermost `Gtk::Box` wrapping an entire section
> (header + content), not just the inner flow box.
 
---
 
## 10. DialogFormHost — Type-Selector Dialogs
 
`DialogFormHost` is an intermediate base for dialogs whose `Data` objects own
child collections and whose UI is driven by a primary type-selector combo
(device type, restrictor type, input type, etc.).
 
**Use `DialogFormHost` when all of these are true:**
- The dialog has a combo that determines which child data/UI is shown.
- Changing that combo selection must wipe existing child data.
- A confirmation dialog is needed when the user changes an already-populated type.
 
**Current users:** `DialogDevice`, `DialogRestrictor`, `DialogInput`,
`DialogInputSource`.
 
### What it provides
 
| Member | Purpose |
|--------|---------|
| `previousName` | Guards against spurious `signal_changed` re-fires. |
| `switchType()` | Calls `currentData->reset()` + `clearForm()` + `refreshBox()`. |
| `handleTypeSwitch()` | Full decision tree for a type-selector combo. Returns `TypeSwitchResult`. |
| `markUsed()` | Updates a liststore's availability column via a predicate. |
 
### `handleTypeSwitch()` decision order
 
1. `name` empty → `clearForm()`, reset `previousName` → return `Empty`.
2. `name == previousName` → return `Unchanged`.
3. No committed data yet → `switchType()` silently → return `Proceed`.
4. `name != stored name` → ask confirmation:
   - yes → `switchType()` → return `Proceed`.
   - no → revert combo to `previousName` → return `Unchanged`.
5. Set `previousName = name` → return `Proceed`.
 
```cpp
comboBoxInputSelectInput->signal_changed().connect([this, btnAdd]() {
    const string name(comboBoxInputSelectInput->get_active_id());
    switch (handleTypeSwitch(comboBoxInputSelectInput, name, "Change type? All settings will be lost.")) {
    case TypeSwitchResult::Empty:
        btnAdd->set_sensitive(false);
        return;
    case TypeSwitchResult::Unchanged:
        return;
    case TypeSwitchResult::Proceed:
        break;
    }
    // Update type-specific UI here...
});
```
 
### Manual use of `previousName` + `switchType()`
 
When `handleTypeSwitch()` doesn't fit (e.g. the guarding condition is based on
child widget count rather than `currentData` field presence), write the signal
handler explicitly using just `previousName` and `switchType()` from the base.
 
**Critical rule:** always set `previousName` **before** calling `switchType()`
or any method that rebuilds the combo (such as `clearForm()` or
`populateSourcesList()`). Rebuilding the combo fires `signal_changed` again; if
`previousName` is not set yet, the new signal sees an apparent name change and
enters confirmation again.
 
```cpp
// CORRECT — previousName set before switchType() rebuilds the combo.
previousName = resolved;
switchType();
comboBoxInputSource->set_active_id(resolved);
 
// WRONG — previousName set after, so the re-fire triggers confirmation.
switchType();
previousName = resolved;
```
 
The same rule applies in `retrieveData()` — set `previousName` before calling
any `set_active_id()` that would fire `signal_changed`:
 
```cpp
void DialogInputSource::retrieveData() {
    const string source(currentData->getValue(SOURCE));
    previousName = source; // MUST come before set_active_id
    if (not comboBoxInputSource->set_active_id(source)) {
        comboBoxInputSource->set_active_id(SOURCE_OTHER_OPTION);
        entryInputSource->set_text(source);
    }
}
```
 
### `switchType()` and the destructor
 
`switchType()` calls `currentData->reset()` which destroys child collections.
Any child dialog whose `items` pointer was wired to one of those collections
will be left with a dangling pointer. To prevent this, the `Data` subclass
that owns child collections **must** override `deActivate()` to null out the
child dialog's `items` pointer before its collections are destroyed.
 
```cpp
// InputSource::activate() wires DialogInputMap.
void InputSource::activate() {
    DialogInputMap::getInstance()->setOwner(&maps, this);
}
 
// InputSource::deActivate() must unwire it.
void InputSource::deActivate() {
    DialogInputMap::getInstance()->setOwner(nullptr, nullptr);
}
 
// And the destructor must call deActivate() explicitly.
InputSource::~InputSource() {
    deActivate();
    // ... other cleanup ...
}
```
 
The destructor must call `deActivate()` because `onDelClicked` does not call
it explicitly — it relies on the destructor to do so (see comment in
`DialogForm::onDelClicked`). The ADD and EDIT flows call `deActivate()`
explicitly after the dialog closes, so the destructor call there is a safe
no-op.
 
### `SOURCELESS` property
 
For inputs that do not need a hardware source, `DialogInputSource` creates a
phantom `InputSource` with `setProperty(SOURCELESS, "true")`. `DialogInputMap`
checks this property in `setOwner()` to route maps into the correct box:
 
```cpp
void DialogInputMap::setOwner(BoxButtonCollection* collection, Data* owner) {
    DialogForm::setOwner(collection, owner);
    box = (owner and owner->hasProperty(SOURCELESS)) ? boxDirectMaps : boxSourceMaps;
}
```
 
---
 
## 11. DialogFileForm — File-Based Dialogs
 
`DialogFileForm` is an intermediate base for dialogs that manage `FileData`
subclasses (`Input`, `Animation`, `Profile`). It adds directory tracking:
 
```cpp
void setCurrentDirectory(Storage::DirectoryEntry* directory);
Storage::DirectoryEntry* getCurrentDirectory() const;
string getFullPath(const string& filename) const;
bool isUniqueFilename(const string& filename) const;
```
 
The navigator sets `currentDirectory` before opening the dialog.
`isUniqueFilename()` scopes the uniqueness check to that directory only — the
same filename is valid in a sibling directory.
 
---
 
## 12. DialogSelect — Picking From Existing Items
 
`DialogSelect` is a specialized, non-`DialogForm` dialog used when items need
to be **chosen from a pre-existing collection** rather than created from
scratch. It creates `Link` objects pointing to the chosen `Data` items.
 
### SettingRequest
 
Before opening, the caller configures `DialogSelect` via a `SettingRequest`
struct:
 
```cpp
const DialogSelect::SettingRequest mySetting {
    workingBox,           // OrdenableFlowBox*& where selected Links will appear
    NAME,                 // attribute key used to serialize the link
    TYPE_ELEMENT,         // XML element name for the resulting Link
    COLLECTION_ELEMENT,   // CollectionHandler name to pick from
    DialogSelect::BUTTON_COLORER | DialogSelect::BUTTON_DELETER
};
```
 
`DialogSelect` rebuilds its display from the collection every time it opens.
Selection is a `FlowBoxChild` wrapping `Data*` without owning it.
 
---
 
## 13. Creating a New Dialog — Step-by-Step
 
### 1 — Choose the base class
 
| Situation | Base |
|-----------|------|
| Dialog has a type-selector combo that wipes child data | `DialogFormHost` |
| Dialog manages files in a directory tree | `DialogFileForm` |
| Dialog picks from existing items | `DialogSelect` (not `DialogForm`) |
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
    Gtk::Entry* entryMyThingName = nullptr;
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
    builder->get_widget("BtnApplyMyThing",    btnApply);
    Gtk::Button* btnAdd = nullptr;
    builder->get_widget("BtnAddMyThing",      btnAdd);
    builder->get_widget("EntryMyThingName",   entryMyThingName);
    setSignalAdd(btnAdd);
    setSignalApply();
}
```
 
### 4 — Register the dialog
 
**Primary** (top-level collection, in `MainWindow`):
 
```cpp
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
 
## 14. Quick Reference — Virtual Methods to Override
 
| Method | Must override? | Purpose |
|--------|---------------|---------|
| `clearForm()` | **Yes** (pure) | Reset all GTK widgets to a blank state. |
| `isValid()` | **Yes** (pure) | Throw `Message` if widget values are invalid. |
| `storeData()` | **Yes** (pure) | Write widget values into `currentData`. |
| `retrieveData()` | **Yes** (pure) | Write `currentData` values into widgets. |
| `createUniqueId()` | **Yes** (pure) | Build a unique ID from current widget values. |
| `getType()` | **Yes** (pure) | Human-readable type name for dialog titles. |
| `createData(StringUMap&)` | **Yes** (pure) | Factory: construct the correct `Data` subclass. |
| `getCollectionHandler()` | **Yes** (pure) | Return the `CollectionHandler` for this dialog's items. |
| `load(XMLHelper*)` | **Yes** (pure) | Dispatch raw XML data into `createItems()`. |
| `resetForm()` | When extra state must be cleared | Calls `clearForm()` by default. |
| `addButtons(BoxButton&)` | When non-default buttons are needed | Default adds Edit + Delete. |
| `createSubItems(XMLHelper*)` | When items have nested children | Called per item during `createItems`. |
| `afterCreate(BoxButton&)` | When something should happen only on ADD | Hook after ADD completes. |
| `afterDeleteConfirmation(BoxButton&)` | When deletion needs extra cleanup | Called before item is removed. |
| `setOwner(collection, owner)` | When extra wiring is needed | Base stores `items` and `ownerData`. |
 
---
 
## 15. Common Pitfalls
 
### Signal re-fires during `retrieveData()`
 
Setting a combo value in `retrieveData()` fires `signal_changed`. If
`previousName` has not been set yet, the signal handler sees an apparent type
change and may show an unwanted confirmation dialog. **Always set `previousName`
before calling `set_active_id()`** in `retrieveData()`.
 
### `show_all()` overrides `hide()`
 
`refreshBox()` ends with `box->show_all()`, which propagates through all
descendants and overrides any prior `hide()` call. For sections that are
conditionally visible:
- Use `set_no_show_all(true)` on the widget if it should never show via `show_all`.
- Always hide the **outermost** container box for an entire section (header +
  content), not just the inner content widget.
 
### Dangling `items` pointer after `reset()`
 
When `switchType()` calls `currentData->reset()` and the `Data` owns child
collections, any child dialog whose `items` points into those collections will
be left with a dangling pointer. The `Data` subclass must override `deActivate()`
to call `setOwner(nullptr, nullptr)` on the child dialog, and its destructor
must call `deActivate()`.
 
### `previousName` must be set before `switchType()`
 
`switchType()` calls `clearForm()` which may repopulate combos, firing
`signal_changed` again. If `previousName` is not yet updated, the re-fire looks
like a new change and triggers confirmation. Set `previousName = resolved`
**before** calling `switchType()`.
 
### Combo vs entry — resolve to one value
 
When a dialog has both a combo and a manual text entry for the same logical
value (e.g. device source), merge them into a single `resolvedSource()` helper
and drive all signal handlers through it. This avoids duplicating the guard
logic and keeps `createUniqueId()` consistent.
