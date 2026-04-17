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

	Defaults::setFilter(filterEntry, pickerBox);

	DialogLinkEditor::buildInstance(builder, "DialogLinkEdit");
}

void DialogSelect::open() noexcept {

	show();
	populatePicker();

	if (run() != Gtk::ResponseType::RESPONSE_APPLY) {
		hide();
		return;
	}

	// Build a fast lookup set of selected target pointers.
	std::unordered_set<const Storage::Data*> selected;
	for (auto flowChild : pickerBox->get_selected_children()) {
		auto sel{static_cast<Storage::Selection*>(flowChild->get_child())};
		selected.insert(sel->getData());
	}

	// Remove deselected Links from destination and displayBox.
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
		if (not destination->isSet(target))
			addLink(target);
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

void DialogSelect::load(XMLHelper* values, const string& ownerUniqueId) noexcept {

	auto& rawCollection = values->getData(
		Defaults::createCommonUniqueId({ownerUniqueId, request->linkType})
	);

	string location(" while loading " + request->linkType + " for " + ownerUniqueId);
	string errors;

	for (auto& rawItem : rawCollection) {
		try {
			if (rawItem.find(request->linkKey) == rawItem.end())
				throw Message("Missing key " + request->linkKey + location);

			const string& keyValue = rawItem.at(request->linkKey);
			Storage::Data* target  = request->sourceCollection->get(keyValue);
			if (not target)
				throw Message(
					"Cannot find " + request->linkType +
					" with " + request->linkKey + " = " + keyValue + location
				);

			rawItem.erase(request->linkKey);
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
			errors += e.getMessage() + '\n';
			Defaults::markDirty();
		}
	}

	request->displayBox->show_all();

	if (not errors.empty())
		Message::displayError("Errors loading " + request->linkType + ":\n" + errors);
}

void DialogSelect::populatePicker() noexcept {

	for (auto child : pickerBox->get_children())
		pickerBox->remove(*child);

	for (const auto& [id, data] : *request->sourceCollection) {
		if (data->getProperties().getValue(PROP_NO_SELECT).empty()) {
			auto selection{Gtk::make_managed<Storage::Selection>(data)};
			auto flowChild{Gtk::make_managed<Gtk::FlowBoxChild>()};

			if (data->getProperties().isSet("system"))
				selection->get_style_context()->add_class("system");

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
		boxButton.pack_start(*btn, Gtk::PACK_SHRINK);
		btn->set_image_from_icon_name("emblem-system-symbolic", Gtk::ICON_SIZE_BUTTON);
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
	boxButton.pack_start(*btn, Gtk::PACK_SHRINK);
	btn->set_image_from_icon_name("edit-delete", Gtk::ICON_SIZE_BUTTON);
	btn->set_tooltip_text("Remove");
	btn->signal_clicked().connect([&boxButton, this]() {
		Defaults::markDirty();
		request->displayBox->remove(boxButton);
		destination->remove(boxButton);
	});

	boxButton.show_all();
}

void DialogSelect::addLink(Storage::Data* target) noexcept {
	StringUMap empty;
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
