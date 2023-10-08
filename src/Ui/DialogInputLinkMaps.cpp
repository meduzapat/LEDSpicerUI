/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputLinkMap.cpp
 * @since     Oct 1, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include "DialogInputLinkMaps.hpp"

using namespace LEDSpicerUI::Ui;

DialogInputLinkMaps* DialogInputLinkMaps::instance = nullptr;

void DialogInputLinkMaps::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance)
		builder->get_widget_derived("DialogInputLinkMaps", instance);
}

DialogInputLinkMaps* DialogInputLinkMaps::getInstance() {
	return instance;
}

void DialogInputLinkMaps::setMappings(const Forms::BoxButtonCollection* maps) {
	inputMaps = maps;
}

DialogInputLinkMaps::DialogInputLinkMaps(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
DialogForm(obj, builder) {

	builder->get_widget_derived("BoxInputLinkedMaps", box);
	builder->get_widget("BtnAddInputLinkedMap",       btnAdd);
	builder->get_widget("BtnApplyInputLinkedMap",     btnApply);

	builder->get_widget("ComboBoxSelectMapping", comboBoxSelectMapping);
	builder->get_widget("BtnAddMapping",         btnAddMapping);
	builder->get_widget_derived(
		"BoxInputLinkedMappings",
		boxInputLinkedMappings,
		"BtnInputMappingUp",
		"BtnInputMappingDown"
	);

	signal_show().connect([&]() {
		mappings.wipe();
		populateMappings();
	});
//onDelClicked(button);
	comboBoxSelectMapping->signal_changed().connect([&]() {
		btnAddMapping->set_sensitive(comboBoxSelectMapping->get_active_text() != "");
	});

	btnAddMapping->signal_clicked().connect([&]() {
		unordered_map<string, string> rawData = {std::make_pair(NAME, comboBoxSelectMapping->get_active_text())};
		auto map = new Forms::NameOnly(rawData, "ElementBoxButton");
		auto bPtr = mappings.add("Map Link", map);
		bPtr->setDelFn([&, bPtr]() {
			boxInputLinkedMappings->remove(*bPtr);
			mappings.remove(bPtr);
		});
		boxInputLinkedMappings->add(*bPtr);
		boxInputLinkedMappings->show_all();
	});

	setSignalAdd();
	setSignalApply();
}

Forms::Form* DialogInputLinkMaps::getForm(unordered_map<string, string>& rawData) {
	return new Forms::NameOnly(rawData);
}

string DialogInputLinkMaps::getType() {
	return "input Link";
}

void DialogInputLinkMaps::populateMappings() {

	vector<string> names;
	comboBoxSelectMapping->remove_all();
	for (auto item : items) {
		names.push_back(item->getForm()->getValue(NAME));
	}
	for (auto& map : *inputMaps) {
		string mapName(map->getForm()->createPrettyName());
		bool found = false;
		for (auto& name : names) {
			if (name.find(mapName) != std::string::npos) {
				found = true;
				break;
			}
		}
		if (not found) {
			comboBoxSelectMapping->append(mapName);
		}
	}
}

void DialogInputLinkMaps::afterCreate(Forms::BoxButton* boxButton) {

}

void DialogInputLinkMaps::afterDeleteConfirmation(Forms::BoxButton* boxButton) {

}
