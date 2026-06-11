/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogPrompt.cpp
 * @since     Jun 6, 2026
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicerUI is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicerUI is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DialogPrompt.hpp"

using namespace LEDSpicerUI::Ui;

DialogPrompt::DialogPrompt(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	GladeDialog(obj, builder)
{
	builder->get_widget("BoxPromptName",    boxName);
	builder->get_widget("BoxPromptGroup",   boxGroup);
	builder->get_widget("BoxPromptElement", boxElement);
	builder->get_widget("EntryPromptName",  entryName);
	builder->get_widget("ComboBoxPromptGN1", comboGN1);
	builder->get_widget("ComboBoxPromptGN2", comboGN2);

	// Element name generator.
	builder->get_widget("StackNameGen",                stack);
	builder->get_widget("StackSwitcherNameGen",        stackSwitcher);
	builder->get_widget("ComboNameGenPlayer",          comboPlayer);
	builder->get_widget("ComboNameGenPlayerType",      comboPlayerType);
	builder->get_widget("ComboNameGenPlayerIndex",     comboPlayerIndex);
	builder->get_widget("ComboNameGenPlayerWays",      comboPlayerWays);
	builder->get_widget("ComboNameGenCabinetCategory", comboCabinetCategory);
	builder->get_widget("ComboNameGenCabinetIndex",    comboCabinetIndex);
	builder->get_widget("LabelNameGenPlayerIndex",     labelPlayerIndex);
	builder->get_widget("LabelNameGenPlayerWays",      labelPlayerWays);
	builder->get_widget("LabelNameGenPreview",         previewLabel);
	builder->get_widget("ImageNameGenPreview",         previewIcon);
	cabinetStore = static_cast<Gtk::ListStore*>(builder->get_object("liststoreNameGenCabinet").get());

	Defaults::attachFilenameFilter(entryName);

	// Header-bar buttons: Cancel left, Apply right (matches DialogImport).
	add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	btnApply = add_button("_Apply", Gtk::ResponseType::RESPONSE_APPLY);
	btnApply->set_can_default(true);

	entryName->set_activates_default(true);

	populateCabinetStore();

	// Every selection change rebuilds the preview.
	for (auto* c : {comboPlayer, comboPlayerIndex, comboPlayerWays, comboCabinetIndex})
		c->signal_changed().connect(sigc::mem_fun(*this, &DialogPrompt::updatePreview));
	comboCabinetCategory->signal_changed().connect(sigc::mem_fun(*this, &DialogPrompt::updatePreview));

	// Player TYPE drives row visibility too.
	comboPlayerType->signal_changed().connect([this]() {
		updatePlayerControlVisibility();
		updatePreview();
	});

	// Tab switch.
	stack->property_visible_child_name().signal_changed().connect(
		sigc::mem_fun(*this, &DialogPrompt::updatePreview)
	);
}

namespace {

struct PlayerType { const char* id; const char* label; };
constexpr PlayerType PLAYER_TYPE_ALL[] = {
	{"BUTTON",     "Button"},
	{"JOYSTICK",   "Joystick"},
	{"MOUSE",      "Mouse"},
	{"LIGHTGUN",   "Light Gun"},
	{"TRACKBALL",  "Trackball"},
	{"DIAL",       "Dial"},
	{"PADDLE",     "Paddle"},
	{"PEDAL",      "Pedal"},
	{"POSITIONAL", "Positional"},
	{"START",      "Start"},
	{"COIN",       "Coin"},
};

struct CabinetRow { const char* id; const char* label; };
constexpr CabinetRow CABINET_GROUPS[][6] = {
	{{nullptr, "Actuators"},   {"KNOCKER",  "Knocker"},               {"MOTOR",    "Motor"},
	 {"PUSHER",  "Pusher"},    {"SOLENOID", "Solenoid"},              {"RECOIL",   "Recoil"}},
	{{nullptr, "LED Strips"},  {"SOUND",    "Sound-reactive strip"},  {"CABINET",  "Cabinet bar"},
	 {"TMOLDING","T-Molding"}, {"MARQUEE",  "Marquee"},               {"FLOOR",    "Under-cabinet glow"}},
	{{nullptr, "Controls"},    {"MENU",     "Menu"},                  {"BACK",     "Back"},
	 {"PAUSE",   "Pause"},     {"ENTER",    "Enter"},                 {nullptr, nullptr}},
	{{nullptr, "System"},      {"POWER",    "Power"},                 {"LIGHT",    "Generic light"},
	 {nullptr, nullptr},       {nullptr, nullptr},                    {nullptr, nullptr}},
};

const std::unordered_map<string, DialogPrompt::Filter> FILTERS {
	{ELEMENT_TYPE_BUTTON,    {true,  false, {"BUTTON"},               false, {}}},
	{ELEMENT_TYPE_JOYSTICK,  {true,  false, {"JOYSTICK"},             false, {}}},
	{ELEMENT_TYPE_TRACKBALL, {true,  false, {"TRACKBALL", "MOUSE"},   false, {}}},
	{ELEMENT_TYPE_SPINNER,   {true,  false, {"DIAL", "PADDLE"},       false, {}}},
	{ELEMENT_TYPE_LIGHTGUN,  {true,  false, {"LIGHTGUN"},             false, {}}},
	{ELEMENT_TYPE_CREDIT,    {true,  false, {"START", "COIN"},        false, {}}},
	{ELEMENT_TYPE_ACTUATOR,  {false, true,  {},                       false,
		{"KNOCKER", "MOTOR", "PUSHER", "SOLENOID", "RECOIL"}}},
	{ELEMENT_TYPE_BAR,       {false, true,  {},                       false,
		{"SOUND", "CABINET", "TMOLDING", "MARQUEE", "FLOOR"}}},
	{ELEMENT_TYPE_LIGHT,     {true,  true,  {"JOYSTICK"},             true,
		{"SOUND", "CABINET", "TMOLDING", "MARQUEE", "FLOOR",
		 "MENU", "BACK", "PAUSE", "ENTER", "POWER", "LIGHT"}}},
	{ELEMENT_TYPE_MISC,      {true,  true,  {},                       true,  {}}},
};

} // anonymous

void DialogPrompt::populateCabinetStore() noexcept {
	for (const auto& group : CABINET_GROUPS) {
		for (const auto& row : group) {
			if (not row.label) continue;
			auto r {*(cabinetStore->append())};
			r.set_value(0, string(row.id ? row.id : ""));
			r.set_value(1, string(row.label));
			r.set_value(2, row.id != nullptr);  // headers stay insensitive forever
		}
	}
}

void DialogPrompt::applyFilter(const string& elementTypeId) noexcept {
	const auto it {FILTERS.find(elementTypeId)};
	currentFilter = (it == FILTERS.end()) ? FILTERS.at(ELEMENT_TYPE_MISC) : it->second;

	// Stack pages.
	stack->get_child_by_name("player")->set_visible(currentFilter.playerTab);
	stack->get_child_by_name("cabinet")->set_visible(currentFilter.cabinetTab);
	stackSwitcher->set_visible(currentFilter.playerTab and currentFilter.cabinetTab);
	stack->set_visible_child(currentFilter.playerTab ? "player" : "cabinet");

	// Rebuild the Player Control TYPE combo with only allowed ids.
	comboPlayerType->remove_all();
	for (const auto& t : PLAYER_TYPE_ALL) {
		if (currentFilter.playerTypes.empty() or currentFilter.playerTypes.count(t.id))
			comboPlayerType->append(t.id, t.label);
	}

	// Cabinet rows — keep headers insensitive; gate entries by the allowed set.
	for (auto iter : cabinetStore->children()) {
		auto row {*iter};
		string id;
		row.get_value(0, id);
		if (id.empty()) continue;  // group header
		const bool allowed {currentFilter.cabinetCategories.empty() or
		                    currentFilter.cabinetCategories.count(id)};
		row.set_value(2, allowed);
	}

	// Reset selections.
	comboPlayer->set_active(-1);
	comboPlayerType->set_active(-1);
	comboPlayerIndex->set_active(-1);
	comboPlayerWays->set_active(-1);
	comboCabinetCategory->set_active(-1);
	comboCabinetIndex->set_active(-1);
}

void DialogPrompt::updatePlayerControlVisibility() noexcept {
	const string t {comboPlayerType->get_active_id()};
	const bool hasIndex {not t.empty() and t != "START" and t != "COIN"};
	labelPlayerIndex->set_visible(hasIndex);
	comboPlayerIndex->set_visible(hasIndex);
	const bool hasWays {currentFilter.playerWaysAllowed and t == "JOYSTICK"};
	labelPlayerWays->set_visible(hasWays);
	comboPlayerWays->set_visible(hasWays);
}

string DialogPrompt::buildPreview() const noexcept {
	const string page {stack->get_visible_child_name()};
	if (page == "player")
		return Defaults::buildPlayerControlName(
			comboPlayer->get_active_id(),
			comboPlayerType->get_active_id(),
			labelPlayerIndex->is_visible() ? string(comboPlayerIndex->get_active_id()) : emptyString,
			labelPlayerWays->is_visible()  ? string(comboPlayerWays->get_active_id())  : emptyString
		);
	if (page == "cabinet")
		return Defaults::buildCabinetItemName(
			comboCabinetCategory->get_active_id(),
			comboCabinetIndex->get_active_id()
		);
	return emptyString;
}

void DialogPrompt::updatePreview() noexcept {
	const string name {buildPreview()};
	previewLabel->set_text(name);
	if (name.empty()) {
		previewIcon->set_from_icon_name("dialog-question-symbolic", Gtk::ICON_SIZE_DIALOG);
		btnApply->set_sensitive(false);
		return;
	}
	auto typeId {Defaults::matchElementTypeByName(name)};
	if (typeId.empty()) typeId = ELEMENT_TYPE_MISC;
	// Mirrors LayoutElement::ICON_DIR — keep until a third caller justifies hoisting.
	previewIcon->set(string{PACKAGE_DATA_DIR} + "images/elements/element-" + typeId + ".png");
	btnApply->set_sensitive(true);
}

int DialogPrompt::runFor(Section section, Gtk::Window* parent) noexcept {
	set_transient_for(parent ? *parent : Message::getMain());
	boxName->hide();
	boxGroup->hide();
	boxElement->hide();
	switch (section) {
	case Section::DirName:
		set_title("Folder Name");
		boxName->show();
		entryName->grab_focus();
		break;
	case Section::GroupName:
		set_title("Generate Group Name");
		boxGroup->show();
		break;
	case Section::ElementName:
		set_title("Generate Element Name");
		boxElement->show();
		break;
	}
	btnApply->grab_default();
	const int response {run()};
	hide();
	return response;
}

string DialogPrompt::askDirName(const string& current, Gtk::Window* parent) noexcept {
	entryName->set_text(current);
	if (runFor(Section::DirName, parent) != Gtk::ResponseType::RESPONSE_APPLY)
		return emptyString;
	return Defaults::sanitizeFilename(entryName->get_text());
}

string DialogPrompt::askGroupName(Gtk::Window* parent) noexcept {
	comboGN1->set_active(-1);
	comboGN2->set_active(-1);
	if (runFor(Section::GroupName, parent) != Gtk::ResponseType::RESPONSE_APPLY)
		return emptyString;
	return comboGN1->get_active_id() + comboGN2->get_active_id();
}

string DialogPrompt::askElementName(const string& elementTypeId, Gtk::Window* parent) noexcept {
	applyFilter(elementTypeId);
	updatePlayerControlVisibility();
	updatePreview();
	if (runFor(Section::ElementName, parent) != Gtk::ResponseType::RESPONSE_APPLY)
		return emptyString;
	return buildPreview();
}
