/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogSelect.cpp
 * @since     Feb 22, 2023
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

#include "DialogSelect.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

DialogSelect::DialogSelect(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	GladeDialog(obj, builder)
{
	Gtk::Button
		* btnSelectAll  = nullptr,
		* btnSelectNone = nullptr;
	Gtk::SearchEntry* filterEntry = nullptr;

	builder->get_widget("BntProcessSelection", btnApply);
	builder->get_widget("BoxAll",              pickerBox);
	builder->get_widget("BtnSelectAll",        btnSelectAll);
	builder->get_widget("BtnSelectNone",       btnSelectNone);
	builder->get_widget("entryFilterSelects",  filterEntry);

	btnSelectAll->signal_clicked().connect(sigc::mem_fun(*pickerBox, &Gtk::FlowBox::select_all));
	btnSelectNone->signal_clicked().connect(sigc::mem_fun(*pickerBox, &Gtk::FlowBox::unselect_all));

	Defaults::setFilter(filterEntry, pickerBox, this);

	DialogLinkEditor::buildInstance(builder, "DialogLinkEdit");
}

DialogSelect::~DialogSelect() {
	delete DialogLinkEditor::getInstance();
}

void DialogSelect::open() noexcept {

	// Dialog need to be realized in order to allow selections on the pre-existing items.
	show();
	populatePicker();

	auto updateApply = [this]() {
		btnApply->set_sensitive(
			static_cast<int>(pickerBox->get_selected_children().size()) >= request->minSelection
		);
	};
	updateApply();
	auto conn = pickerBox->signal_selected_children_changed().connect(updateApply);

	if (run() != Gtk::ResponseType::RESPONSE_APPLY) {
		conn.disconnect();
		btnApply->set_sensitive(true);
		hide();
		return;
	}
	conn.disconnect();

	// Build a fast lookup set of selected target pointers.
	std::unordered_set<const Storage::Data*> selected;
	for (auto flowChild : pickerBox->get_selected_children()) {
		auto sel{static_cast<Storage::Selection*>(flowChild->get_child())};
		selected.insert(sel->getData());
	}

	// Remove de-selected Links from destination and displayBox.
	vector<Storage::BoxButton*> toRemove;
	for (auto btn : *destination) {
		if (not selected.count(btn->getData()))
			toRemove.push_back(btn);
	}
	for (auto btn : toRemove) {
		request->displayBox->remove(*btn);
		destination->remove(*btn);
	}

	// Add newly selected items not already in destination.
	for (auto flowChild : pickerBox->get_selected_children()) {
		auto target{static_cast<Storage::Selection*>(flowChild->get_child())->getData()};
		if (not destination->isSet(target)) {
			Values empty;
			auto link{new Storage::Link(
				empty,
				request->linkKey,
				request->linkType,
				request->linkFields,
				target
			)};

			for (const auto& field : request->linkFields)
				link->setValue(field.key, field.defaultValue);

			Storage::BoxButton& btn = destination->create(link);
			addDisplayButtons(btn);
			request->displayBox->add(btn);
		}
	}

	request->displayBox->show_all();
	Defaults::markDirty();
	hide();
}

void DialogSelect::refresh() noexcept {
	request->displayBox->wipe();
	for (auto btn : *destination)
		request->displayBox->add(*btn);
	request->displayBox->show_all();
}

void DialogSelect::reindex() noexcept {
	destination->reindex(request->displayBox);
}

void DialogSelect::load(DataMap& values, const string& ownerUniqueId, const string& ownerName) noexcept {

	auto& rawCollection{values[
		Defaults::createCommonUniqueId({ownerUniqueId, request->collectionId})
	]};

	string errors;

	for (auto& rawItem : rawCollection) {
		try {

			const string& keyValue {rawItem.getValue(request->linkKey)};
			Storage::Data* target  {request->sourceCollection->get(keyValue)};
			if (not target)
				throw Message(
					request->linkType + " '" + keyValue + "' not found"
				);

			rawItem.unSet(request->linkKey);
			auto link{new Storage::Link(
				rawItem,
				request->linkKey,
				request->linkType,
				request->linkFields,
				target
			)};
			Storage::BoxButton& btn = destination->create(link);
			addDisplayButtons(btn);
			request->displayBox->add(btn);
		}
		catch (Message& e) {
			errors += e.takeMessage() + '\n';
		}
	}

	request->displayBox->show_all();

	if (not errors.empty()) {
		const string scope {ownerName.empty() ? ownerUniqueId : ownerName};
		if (Message::isBatching())
			Message::collect("In " + scope + ", missing " + request->linkType + " links:\n" + errors);
		else
			Message::displayError("Errors loading " + request->linkType + ":\n" + errors, this);
	}
}

void DialogSelect::populatePicker() noexcept {

	for (auto child : pickerBox->get_children())
		pickerBox->remove(*child);

	for (const auto& [id, data] : *request->sourceCollection) {
		if (data->getProperties().getValue(PROP_NO_SELECT).empty()) {

			if (not request->filterProp.empty() and data->getProperties().getValue(request->filterProp) != request->filterValue)
				continue;

			auto selection{Gtk::make_managed<Storage::Selection>(data)};
			auto flowChild{Gtk::make_managed<Gtk::FlowBoxChild>()};

			if (data->getProperties().isSet(PROP_SYSTEM))
				selection->get_style_context()->add_class(CSS_SYSTEM);

			selection->signal_clicked().connect([this, flowChild]() {
				if (flowChild->is_selected())
					pickerBox->unselect_child(*flowChild);
				else
					pickerBox->select_child(*flowChild);
			});

			flowChild->add(*selection);
			pickerBox->add(*flowChild);

			if (destination->isSet(data))
				pickerBox->select_child(*flowChild);
		}
	}

	pickerBox->show_all();
}

void DialogSelect::addDisplayButtons(Storage::BoxButton& boxButton) noexcept {

	// EDIT — only when this link type carries extra fields.
	if (not request->linkFields.empty()) {
		auto btn{Gtk::make_managed<Gtk::Button>()};
		boxButton.packButtonStart(*btn);
		btn->set_image_from_icon_name(ICON_EDIT, Gtk::ICON_SIZE_BUTTON);
		btn->get_style_context()->add_class(CSS_BOX_BACKGROUND_EDIT);
		btn->set_tooltip_text("Edit");
		btn->signal_clicked().connect([&boxButton, this]() {
			DialogLinkEditor::getInstance()->open(
				static_cast<Storage::Link*>(boxButton.getData())
			);
			boxButton.sync();
		});
	}

	// DELETE its always present.
	auto btn{Gtk::make_managed<Gtk::Button>()};
	boxButton.packButtonStart(*btn);
	btn->set_image_from_icon_name(ICON_DELETE, Gtk::ICON_SIZE_BUTTON);
	btn->get_style_context()->add_class(CSS_BOX_BACKGROUND_DELETE);
	btn->set_tooltip_text("Remove");
	btn->signal_clicked().connect([&boxButton, this]() {
		Defaults::markDirty();
		request->displayBox->remove(boxButton);
		destination->remove(boxButton);
	});

	boxButton.show_all();
}

LEDSpicerUI::StringVector DialogSelect::getSelectedIndexes() const noexcept {
	StringVector result;
	if (not request or not destination) return result;
	int pickerIdx = 0;
	for (const auto& [id, data] : *request->sourceCollection) {
		if (data->getProperties().getValue(PROP_NO_SELECT).empty()) {
			if (not request->filterProp.empty() and
				data->getProperties().getValue(request->filterProp) != request->filterValue
			) continue;
			if (destination->isSet(data))
				result.push_back(std::to_string(pickerIdx));
			++pickerIdx;
		}
	}
	return result;
}

void DialogSelect::selectByIndexes(const StringVector& indexes) noexcept {

	if (indexes.empty()) return;

	std::unordered_set<int> targets;
	for (const auto& s : indexes) {
		try { targets.insert(std::stoi(s)); }
		catch (...) {}
	}
	if (targets.empty()) return;

	int idx = 0;
	for (const auto& [id, data] : *request->sourceCollection) {
		if (data->getProperties().getValue(PROP_NO_SELECT).empty()) {
			if (
				not request->filterProp.empty() and
				data->getProperties().getValue(request->filterProp) != request->filterValue
			) continue;
			if (targets.count(idx) and not destination->isSet(data)) {
				Values empty;
				auto link{new Storage::Link(
					empty,
					request->linkKey,
					request->linkType,
					request->linkFields,
					data
				)};
				for (const auto& field : request->linkFields)
					link->setValue(field.key, field.defaultValue);
				Storage::BoxButton& btn = destination->create(link);
				addDisplayButtons(btn);
				request->displayBox->add(btn);
			}
			++idx;
		}
	}
	request->displayBox->show_all();
	Defaults::markDirty();
}
