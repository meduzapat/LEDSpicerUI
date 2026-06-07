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
	builder->get_widget("ComboBoxPromptEN1", comboEN1);
	builder->get_widget("ComboBoxPromptEN2", comboEN2);
	builder->get_widget("ComboBoxPromptEN3", comboEN3);
	builder->get_widget("ComboBoxPromptEN4", comboEN4);

	Defaults::attachFilenameFilter(entryName);

	// Header-bar buttons: Cancel left, Apply right (matches DialogImport).
	add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	btnApply = add_button("_Apply", Gtk::ResponseType::RESPONSE_APPLY);
	btnApply->set_can_default(true);

	entryName->set_activates_default(true);

	// Cascading sensitivity for the element-name combos.
	comboEN1->signal_changed().connect([this]() {
		comboEN2->set_sensitive(not comboEN1->get_active_id().empty());
	});
	comboEN2->signal_changed().connect([this]() {
		comboEN3->set_sensitive(not comboEN2->get_active_id().empty());
	});
	comboEN3->signal_changed().connect([this]() {
		comboEN4->set_sensitive(not comboEN3->get_active_id().empty());
	});
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

string DialogPrompt::askElementName(Gtk::Window* parent) noexcept {
	comboEN1->set_active(-1);
	comboEN2->set_active(-1);
	comboEN3->set_active(-1);
	comboEN4->set_active(-1);
	comboEN2->set_sensitive(false);
	comboEN3->set_sensitive(false);
	comboEN4->set_sensitive(false);
	if (runFor(Section::ElementName, parent) != Gtk::ResponseType::RESPONSE_APPLY)
		return emptyString;
	string name(comboEN1->get_active_id() + comboEN2->get_active_id());
	if (name.empty()) return emptyString;
	if (not comboEN3->get_active_id().empty())
		name += '_' + comboEN3->get_active_id() + comboEN4->get_active_id();
	return name;
}
