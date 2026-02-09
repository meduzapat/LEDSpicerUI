/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputListener.cpp
 * @since     Apr 20, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "DialogInputListener.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogInputListener* DialogInputListener::instance = nullptr;

void DialogInputListener::initialize(Glib::RefPtr<Gtk::Builder> const &builder) {
	if (not instance) {
		builder->get_widget_derived("DialogInputListener", instance);
	}
}

DialogInputListener* DialogInputListener::getInstance() {
	return instance;
}

DialogInputListener::DialogInputListener(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	builder->get_widget_derived("BoxInputListener", box);
	builder->get_widget("BtnApplyInputListener",    btnApply);
	Gtk::Button* btnAdd = nullptr;
	builder->get_widget("BtnAddInputListener", btnAdd);
	setSignalAdd(btnAdd);
	setSignalApply();

	builder->get_widget("EntryInputListenEvent", EntryInputListenEvent);
}

void DialogInputListener::load(XMLHelper *values) {
	createItems(values->getData(COLLECTION_INPUT_EVENTS), values);
}

LEDSpicerUI::Ui::Storage::CollectionHandler* DialogInputListener::getCollectionHandler() const {
	return LEDSpicerUI::Ui::Storage::CollectionHandler::getInstance(COLLECTION_INPUT_EVENTS);
}

void DialogInputListener::clearForm() {
	EntryInputListenEvent->set_text("");
}

void DialogInputListener::isValid() const {
	if (EntryInputListenEvent->get_text().empty()) {
		throw Message("Enter a valid event listener name.");
	}
}
void DialogInputListener::storeData() {

}

void DialogInputListener::retrieveData() {

}

const string DialogInputListener::createUniqueId() const {
	return EntryInputListenEvent->get_text();
}

void DialogInputListener::createSubItems(XMLHelper *values) {

}

const string DialogInputListener::getType() const {
	return "InputListener";
}

LEDSpicerUI::Ui::Storage::Data* DialogInputListener::createData(StringUMap &rawData) {
	return nullptr;
}
