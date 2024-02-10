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

#include "DialogSelectElements.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogSelectElements* DialogSelectElements::instance = nullptr;

void DialogSelectElements::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogSelectElements", instance);
	}
}

DialogSelectElements* DialogSelectElements::getInstance() {
	return instance;
}

DialogSelectElements::DialogSelectElements(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
DialogForm(obj, builder)
{
	Gtk::Button
		* btnSelectAll,
		* btnSelectNone;
	builder->get_widget_derived("BoxGroupElements", box, "BtnGroupElementUp", "BtnGroupElementDn");
	builder->get_widget("BtnAddGroupElements", btnAdd);
	builder->get_widget("BntElementsSelect",   btnApply);
	builder->get_widget("BoxAllElements",      boxAllElements);
	builder->get_widget("BtnSelectAll",        btnSelectAll);
	builder->get_widget("BtnSelectNone",       btnSelectNone);

	btnAdd->signal_clicked().connect([&]() {
		// Run Dialog.
		if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
			clearFormOthers();
			for (auto child : boxAllElements->get_selected_children()) {
				auto b = dynamic_cast<Gtk::Button*>(child->get_child());
				auto l = b->get_label();
				unordered_map<string, string> rawData{{NAME, l}};
				auto element = getData(rawData);
				// Store.
				auto bPtr = items->add(element);
				bPtr->setDelFn([&, bPtr]() {
					box->remove(*bPtr);
					items->remove(bPtr);
				});
				box->add(*bPtr);
			}
			box->show_all();
		}
		hide();
	});

	btnSelectAll->signal_clicked().connect([&]() {
		boxAllElements->select_all();
	});

	btnSelectNone->signal_clicked().connect([&]() {
		boxAllElements->unselect_all();
	});

	signal_show().connect([&]() {
		populateElements();
	});

	btnApply->signal_clicked().connect([&]() {
		try {
			if (not getNumberOfSelectedElements())
				throw Message("Please Select at least one Element");
			response(Gtk::RESPONSE_APPLY);
		}
		catch (Message& e) {
			e.displayError(this);
		}
	});
}

void DialogSelectElements::load(XMLHelper* values) {
	createItems(values->getData(owner->createUniqueId() + COLLECTION_GROUP), values);
}

void DialogSelectElements::clearFormOthers() {
	box->wipe();
	items->wipe();
}

const size_t DialogSelectElements::getNumberOfSelectedElements() const {
	return boxAllElements->get_selected_children().size();
}

const size_t DialogSelectElements::getNumberElements() const {
	return box->get_children().size();
}

const string DialogSelectElements::getType() const {
	return "element";
}

LEDSpicerUI::Ui::Storage::Data* DialogSelectElements::getData(unordered_map<string, string>& rawData) {
	return new Storage::NameOnly(rawData, "ElementBoxButton");
}

void DialogSelectElements::populateElements() {
	for (auto child : boxAllElements->get_children()) {
		boxAllElements->remove(*child);
	}
	for (auto& e : *Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)) {
		auto b = Gtk::make_managed<Gtk::Button>(e);
		b->get_child()->set_halign(Gtk::Align::ALIGN_START);
		auto c = Gtk::make_managed<Gtk::FlowBoxChild>();
		b->signal_clicked().connect([&, c, b]() {
			bool a = c->is_selected();
			if (a)
				boxAllElements->unselect_child(*c);
			else
				boxAllElements->select_child(*c);
			b->grab_focus();
		});
		c->add(*b);
		boxAllElements->add(*c);
		// activate selected.
		if (elementExist(e))
			boxAllElements->select_child(*c);

	}
	boxAllElements->show_all();
}

bool DialogSelectElements::elementExist(const string& name) {
	for (auto b : *items)
		if (b->getData()->getValue(NAME) == name)
			return true;
	return false;
}
