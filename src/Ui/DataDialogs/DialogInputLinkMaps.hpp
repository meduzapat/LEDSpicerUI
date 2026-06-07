/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputLinkMaps.hpp
 * @since     Oct 1, 2023
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

#include "Storage/InputMapLink.hpp"
#include "SortableFlowBox.hpp"
#include "Storage/BoxButtonCollection.hpp"
#include "DialogSelect.hpp"
#include "DialogForm.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInputLinkMaps
 * Manages InputMapLink objects for an Input.
 */
	class DialogInputLinkMaps : public DialogForm, public SingletonDialog<DialogInputLinkMaps> {

		friend class Gtk::Builder;

	public:

		~DialogInputLinkMaps() = default;

		void load(DataMap& values)    noexcept override;
		void clearForm()              noexcept override;
		void storeData()              noexcept override;
		void retrieveData()           noexcept override;
		void isValid()          const          override;
		string createUniqueId() const noexcept override;
		void setOwner(Storage::BoxButtonCollection* collection, Storage::Data* owner) noexcept override;

	protected:

		/// Ordered display box for the current IML's map sequence.
		SortableFlowBox* boxInputLinkedMappings = nullptr;

		/// Configuration for the map link selector.
		DialogSelect::SelectionRequest mapsRequest;

		DialogInputLinkMaps(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

		const string& getType() const noexcept override { return TYPE_INPUT_LINKMAP; }

		Storage::Data* createData(Values& rawData) const noexcept override;

		void createSubItems(DataMap&)    noexcept override;
		void wireChildrenDialogs()       noexcept override;
		void disconnectChildrenDialogs() noexcept override;

	};

	} // namespace
