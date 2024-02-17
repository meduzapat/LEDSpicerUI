/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelect.cpp
 * @since     Feb 22, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "DialogSelect.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogSelect* DialogSelect::instance = nullptr;

void DialogSelect::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogSelect", instance);
	}
}

DialogSelect* DialogSelect::getInstance() {
	return instance;
}

DialogSelect::DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
DialogForm(obj, builder)
{
	Gtk::Button
		* btnSelectAll,
		* btnSelectNone;

	// Box and btnAdd will be set on destination.
	builder->get_widget("BntProcessSelection", btnApply);
	builder->get_widget("BoxAll",              boxAll);
	builder->get_widget("BtnSelectAll",        btnSelectAll);
	builder->get_widget("BtnSelectNone",       btnSelectNone);

	btnSelectAll->signal_clicked().connect([&]() {
		boxAll->select_all();
	});

	btnSelectNone->signal_clicked().connect([&]() {
		boxAll->unselect_all();
	});

	signal_show().connect([&]() {
		populateSelectables();
	});

	setSignalApply();
}

void DialogSelect::RunDialog() {
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		clearFormOthers();
		for (auto child : boxAll->get_selected_children()) {
			auto b = dynamic_cast<Gtk::Button*>(child->get_child());
			auto l = b->get_label();
			unordered_map<string, string> rawData{{NAME, l}};
			auto selected = getData(rawData);
			// Store.
			auto bPtr = items->add(selected);
			addButtons(bPtr);
			box->add(*bPtr);
		}
		box->show_all();
	}
	hide();
}

void DialogSelect::load(XMLHelper* values) {
	// to avoid an error about empty selection, add 1 element and select it.
	populateSelectables();
	boxAll->select_all();
	createItems(values->getData(Defaults::createCommonUniqueId({owner->createUniqueId(), collection})), values);
}

void DialogSelect::setDestinationSettings(
	OrdenableFlowBox* box,
	const string& type,
	const string& collection,
	uint8_t buttons
) {
	this->box        = box;
	this->type       = type;
	this->collection = collection;
	this->buttons    = buttons;
}

void DialogSelect::clearFormOthers() {
	box->wipe();
	items->wipe();
}

void DialogSelect::isValid() const {
	if (not getNumberOfSelections())
		throw Message("Please Select at least one");
};

const size_t DialogSelect::getNumberOfSelections() const {
	return boxAll->get_selected_children().size();
}

const size_t DialogSelect::getNumberOfSelectables() const {
	return box->get_children().size();
}

const string DialogSelect::getType() const {
	return type;
}

LEDSpicerUI::Ui::Storage::Data* DialogSelect::getData(unordered_map<string, string>& rawData) {
	return new Storage::NameOnly(rawData, type + "BoxButton");
}

void DialogSelect::createColorButton(Storage::BoxButton* boxButton) {
	auto button(Gtk::make_managed<Gtk::Button>());
	boxButton->pack_start(*button, Gtk::PACK_SHRINK);
	DialogColors::getInstance()->activateColorButton(button);
}

void DialogSelect::addButtons(Storage::BoxButton* boxButton) {
	if (buttons & DialogSelect::EDITER)
		createEditButton(boxButton);
	if (buttons & DialogSelect::COLORER)
		createColorButton(boxButton);
	if (buttons & DialogSelect::DELETER)
		createDeleteButton(boxButton, false);
	boxButton->show_all();
}

void DialogSelect::populateSelectables() {
	for (auto child : boxAll->get_children()) {
		boxAll->remove(*child);
	}
	for (auto& e : *Storage::CollectionHandler::getInstance(type)) {
		auto b = Gtk::make_managed<Gtk::Button>(e);
		b->get_child()->set_halign(Gtk::Align::ALIGN_START);
		auto c = Gtk::make_managed<Gtk::FlowBoxChild>();
		b->signal_clicked().connect([&, c, b]() {
			bool a = c->is_selected();
			if (a)
				boxAll->unselect_child(*c);
			else
				boxAll->select_child(*c);
			b->grab_focus();
		});
		c->add(*b);
		boxAll->add(*c);
		// activate selected.
		if (exist(e))
			boxAll->select_child(*c);

	}
	boxAll->show_all();
}

bool DialogSelect::exist(const string& name) {
	for (auto b : *items)
		if (b->getData()->getValue(NAME) == name)
			return true;
	return false;
}
