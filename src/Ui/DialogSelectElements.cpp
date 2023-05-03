/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelect.cpp
 * @since     Feb 22, 2023
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

#include "DialogSelectElements.hpp"

using namespace LEDSpicerUI::Ui;

DialogSelectElements* DialogSelectElements::dse = nullptr;

DialogSelectElements* DialogSelectElements::getInstance() {
	return dse ? dse : throw Message("Dialog Select Elements not initialized.");
}

DialogSelectElements* DialogSelectElements::getInstance(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not dse)
		builder->get_widget_derived("DialogSelectElements", dse);
	return getInstance();
}

DialogSelectElements::DialogSelectElements(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{
	builder->get_widget_derived("BoxGroupElements", box, "BtnGroupElementUp", "BtnGroupElementDn");
	builder->get_widget("BtnAddGroupElements", btnAdd);
	builder->get_widget("BntElementsSelect",   btnApply);
	builder->get_widget("BoxAllElements",      boxAllElements);

	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogSelectElements::onAddClicked));

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

size_t DialogSelectElements::getNumberOfSelectedElements() {
	return boxAllElements->get_selected_children().size();
}

size_t DialogSelectElements::getNumberElements() {
	return box->get_children().size();
}

void DialogSelectElements::onAddClicked() {
	// Run Dialog.
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		wipeContents();
		for (auto child : boxAllElements->get_selected_children()) {
			auto b = dynamic_cast<Gtk::Button*>(child->get_child());
			auto l = b->get_label();
			unordered_map<string, string> rawData = {std::make_pair(NAME, l)};
			auto element = getForm(rawData);
			// Store.
			auto bPtr = items.add(getType(), element);
			bPtr->setDelFn([&, bPtr]() {
				box->remove(*bPtr);
				items.remove(bPtr);
			});
			box->add(*bPtr);
		}
		box->show_all();
	}
	hide();
}

string DialogSelectElements::getType() {
	return "element";
}

Forms::Form* DialogSelectElements::getForm(unordered_map<string, string>& rawData) {
	return new Forms::NameOnly(rawData, "ElementBoxButton");
}

void DialogSelectElements::populateElements() {
	for (auto child : boxAllElements->get_children()) {
		boxAllElements->remove(*child);
	}
	for (auto& e : *Forms::CollectionHandler::getInstance("elements")) {
		auto b = Gtk::make_managed<Gtk::Button>(e);
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
	for (auto b : items)
		if (b->getForm()->getValue(NAME) == name)
			return true;
	return false;
}
