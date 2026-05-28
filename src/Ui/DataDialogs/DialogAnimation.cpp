/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogAnimation.cpp
 * @since     Feb 14, 2023
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

#include "DialogAnimation.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogAnimation::DialogAnimation(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	DialogForm(obj, builder)
{
	registerChildDialog<DialogActor>(builder, "DialogActor", COLLECTION_ACTORS);

	Gtk::Button* btnAddAnimation = nullptr;
	builder->get_widget_derived("BoxAnimations", box);
	builder->get_widget("BtnAddAnimation",       btnAddAnimation);
	builder->get_widget("BtnApplyAnimation",     btnApply);
	builder->get_widget("EntryAnimationName",    entryAnimationName);

	setSignalAdd(btnAddAnimation);
	setSignalApply();
}

void DialogAnimation::load(DataMap& values) noexcept {
	createItems(
		values[Defaults::createCommonUniqueId({currentDirectory->getFullPath(), COLLECTION_ANIMATIONS})],
		values
	);
}

void DialogAnimation::createSubItems(DataMap& values) noexcept {
	DialogActor::getInstance()->load(values);
}

void DialogAnimation::clearForm() noexcept {
	entryAnimationName->set_text(emptyString);
}

void DialogAnimation::isValid() const {

	const string filename(entryAnimationName->get_text());
	if (filename.empty()) {
		if (action != Actions::LOAD) entryAnimationName->grab_focus();
		throw Message("Invalid name.");
	}

	const string uid(createUniqueId());
	if (currentData->getCollectionHandler()->isIdSet(uid)) {
		if (action != Actions::EDIT or currentData->createUniqueId() != uid) {
			if (action != Actions::LOAD) entryAnimationName->grab_focus();
			throw Message("Name already in use in this directory.");
		}
	}

	if (action != Actions::LOAD) {
		if (not DialogActor::getInstance()->getBox()->getSize())
			throw Message("Add at least one actor.");
	}
}

void DialogAnimation::storeData() noexcept {
	currentData->getProperties().setValue(FILENAME, entryAnimationName->get_text());
}

void DialogAnimation::retrieveData() noexcept {
	entryAnimationName->set_text(currentData->getProperties().getValue(FILENAME));
}

string DialogAnimation::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		currentData->getProperties().getValue(PID),
		entryAnimationName->get_text()
	});
}

LEDSpicerUI::Ui::Storage::Data* DialogAnimation::createData(Values& rawData) const noexcept {
	auto* data {new Storage::Animation(rawData, currentDirectory)};
	if (action == Actions::LOAD)
		data->getProperties().setValue(PATH_BASE, data->getValue(PATH_BASE));
	return data;
}
