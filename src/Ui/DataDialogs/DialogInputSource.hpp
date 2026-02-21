/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInputSource.hpp
 * @since     Apr 20, 2025
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

#include "DialogForm.hpp"
#include "DialogInputMap.hpp"
#include "Storage/InputSource.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInputSource
 * Dialog to add/edit event sources (hardware input devices) for multi-source plugins.
 * When running locally, scans /dev/input/ for event devices and populates a combo.
 * Selecting "Other" or running in portable mode falls back to a manual text entry.
 */
class DialogInputSource : public DialogForm {

	friend class Gtk::Builder;

public:

	static constexpr const char* SOURCE_EMPTY_OPTION = "Select Source";
	/// Sentinel combo value that activates manual entry.
	static constexpr const char* SOURCE_OTHER_OPTION = "Other";
	/// Linux event device directory.
	static constexpr const char* DEV_INPUT = "/dev/input/";

	DialogInputSource() = delete;

	virtual ~DialogInputSource() = default;

	static void initialize(Glib::RefPtr<Gtk::Builder> const& builder);
	static DialogInputSource* getInstance();
	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
	void clearForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

protected:

	static DialogInputSource* instance;

	Gtk::ComboBoxText
		/// Stores the input in use.
		* comboBoxInputSelectInput = nullptr,
		/// Detected event devices combo (shown when running locally).
		* comboBoxInputSource      = nullptr;

	/// Manual entry (shown in portable mode or when "Other" is selected).
	Gtk::Entry* entryInputSource = nullptr;

	/// To manipulate the maps accessibility when the source is being entered.
	Gtk::Button * btnAddMap = nullptr;

//	OrdenableFlowBox* boxInputMap = nullptr;

	DialogInputSource(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* createData(StringUMap& rawData) override;

private:

	/**
	 * Scans /dev/input/ for event* device files using Gio.
	 *
	 * @return Sorted list of event device names (e.g. "event0", "event1").
	 */
	StringVector scanEventDevices();

	/**
	 * Populates the combo with detected sources plus the "Other" sentinel,
	 * then shows/hides combo and entry according to the result.
	 *
	 * @param sources List.
	 */
	void populateSourcesList(const StringVector& devices);

};

} // namespace
