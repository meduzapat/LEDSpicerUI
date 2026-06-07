/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogTransition.cpp
 * @since     Jun 2026
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

#include "DialogTransition.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogTransition::DialogTransition(
	BaseObjectType*                   obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	GladeDialog(obj, builder)
{
	builder->get_widget("ComboTransitionName",  comboName);
	builder->get_widget("ComboTransitionSpeed", comboSpeed);
	builder->get_widget("BtnTransitionColor",   btnColor);
	builder->get_widget("BoxTransitionSpeed",   boxSpeed);
	builder->get_widget("BoxTransitionColor",   boxColor);
	builder->get_widget("BriefTransition",      brief);

	add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	btnApply = add_button("_Apply", Gtk::ResponseType::RESPONSE_APPLY);

	DialogColors::getInstance()->activateColorButton(btnColor);

	populateTypes();

	comboName->signal_changed().connect([this]() { updateVisibility(); });

	btnApply->signal_clicked().connect([this]() {
		try {
			isValid();
			response(Gtk::ResponseType::RESPONSE_APPLY);
		}
		catch (Message& e) {
			e.displayError(this);
		}
	});
}

void DialogTransition::populateTypes() noexcept {
	listStore = static_cast<Gtk::ListStore*>(comboName->get_model().get());
	auto row = *(listStore->append());
	row.set_value(0, emptyString);
	row.set_value(1, string("None (Instant)"));
	for (const auto& [id, info] : Defaults::transitionsInfo) {
		row = *(listStore->append());
		row.set_value(0, id);
		row.set_value(1, info.name);
	}
}

void DialogTransition::edit(Storage::Transition* transition) noexcept {
	currentData = transition;
	set_title("Profile Transition");
	retrieveData();
	updateVisibility();
	if (run() == Gtk::ResponseType::RESPONSE_APPLY) {
		storeData();
		Defaults::markDirty();
	}
	currentData = nullptr;
	hide();
}

void DialogTransition::retrieveData() noexcept {
	const string& name {currentData->getValue(NAME)};
	comboName->set_active_id(name);
	if (name.empty())
		comboName->set_active(0);
	comboSpeed->set_active_id(currentData->getValue(SPEED, DEFAULT_TRANSITION_SPEED));
	DialogColors::getInstance()->colorizeButton(
		btnColor,
		currentData->getValue(COLOR, DEFAULT_TRANSITION_COLOR)
	);
}

void DialogTransition::storeData() noexcept {
	const string name {comboName->get_active_id()};
	currentData->wipe();
	if (name.empty()) return;
	currentData->setValue(NAME, name);
	if (Defaults::transitionHasFlag(name, Defaults::TRANS_HAS_SPEED))
		currentData->setValue(SPEED, comboSpeed->get_active_id());
	if (Defaults::transitionHasFlag(name, Defaults::TRANS_HAS_COLOR))
		currentData->setValue(COLOR, btnColor->get_label());
}

void DialogTransition::updateVisibility() noexcept {
	const string name {comboName->get_active_id()};
	if (name.empty()) {
		boxSpeed->hide();
		boxColor->hide();
		brief->set_text("Switches profiles instantly with no effect.");
		return;
	}
	boxSpeed->set_visible(Defaults::transitionHasFlag(name, Defaults::TRANS_HAS_SPEED));
	boxColor->set_visible(Defaults::transitionHasFlag(name, Defaults::TRANS_HAS_COLOR));
	auto it {Defaults::transitionsInfo.find(name)};
	brief->set_text(it != Defaults::transitionsInfo.end() ? it->second.brief : emptyString);
}

void DialogTransition::isValid() const {
	const string name {comboName->get_active_id()};
	if (name.empty()) return;
	if (Defaults::transitionHasFlag(name, Defaults::TRANS_HAS_SPEED) and comboSpeed->get_active_id().empty())
		throw Message("Missing transition speed.");
	if (Defaults::transitionHasFlag(name, Defaults::TRANS_HAS_COLOR) and btnColor->get_label().empty())
		throw Message("Missing transition color.");
}
