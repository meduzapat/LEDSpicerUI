/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProcess.cpp
 * @since     May 3, 2023
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

#include "DialogProcess.hpp"

using namespace LEDSpicerUI::Ui;

DialogProcess::DialogProcess(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) :
	DialogForm(obj, builder)
{

	// Connect Process Box and buttons.
	builder->get_widget_derived("BoxProcess", box);
	builder->get_widget("BtnAddProcess",      btnAdd);
	builder->get_widget("BtnApplyProcess",    btnApply);
	btnAdd->signal_clicked().connect(sigc::mem_fun(*this, &DialogProcess::onAddClicked));
	setSignalApply();

	// Process fields.
	builder->get_widget("InputProcessName", fields.InputProcessName);
	builder->get_widget("InputSystemType",  fields.InputSystemType);
	builder->get_widget("InputRomPosition", fields.InputRomPosition);
	Forms::Process::initialize(fields);

	// Process Fields.
	builder->get_widget("InputRunEvery", InputRunEvery);
	Defaults::registerWidget(InputRunEvery);
}

void DialogProcess::setRunEvery(const string& millisencods) {
	if (Defaults::isNumber(millisencods))
		InputRunEvery->set_text(millisencods);
	else
		InputRunEvery->set_text(DEFAULT_RUNEVERY);
}

string DialogProcess::toXml() {
	if (not items.size() or InputRunEvery->get_text().empty())
		return "";

	string r(Defaults::tab() + "<processLookup runEvery=\"" + InputRunEvery->get_text() + "\">\n");
	Defaults::increaseTab();
	for (auto b : items) {
		r += b->toXML("");
	}
	Defaults::reduceTab();
	return (r + Defaults::tab() + "</processLookup>\n");
}

string DialogProcess::getType() {
	return "map";
}

Forms::Form* DialogProcess::getForm(unordered_map<string, string>& rawData) {
	return new Forms::Process(rawData);
}
