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
class DialogInputSource : public DialogForm, public SingletonDialog<DialogInputSource> {

	friend class Gtk::Builder;

public:

	static constexpr const char* SOURCE_EMPTY_OPTION = "Select Source";
	/// Sentinel combo value that activates manual entry.
	static constexpr const char* SOURCE_OTHER_OPTION = "Other";
	/// Linux event device directory.
	static constexpr const char* DEV_INPUT       = "/dev/input/";
	static constexpr const char* DEV_INPUT_BY_ID = "/dev/input/by-id/";
	static constexpr const char* SYS_CLASS_INPUT = "/sys/class/input/";

	virtual ~DialogInputSource();

	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
	void clearForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

	/**
	 * Ensures a phantom source (source="") exists for single-source plugins.
	 * Creates one silently if absent, then wires DialogInputMap to it.
	 * Safe to call repeatedly — idempotent if phantom already exists.
	 */
	void createButtonDirectly();

protected:

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
	 * Populates the combo with detected sources plus the "Other" sentinel,
	 * then shows/hides combo and entry according to the result.
	 *
	 * @param sources List.
	 */
	void populateSourcesList(const StringVector& devices);

	/**
	 * Resolves a device entry to its kernel human-readable name via sysfs.
	 * Works for both by-id symlink names and raw eventX names.
	 * @param  byIdName Entry from /dev/input/by-id/ or a plain eventX name.
	 * @return Kernel name string, or empty on any failure.
	 */
	static string readDeviceName(const string& byIdName);

	/**
	 * Scans /dev/input/by-id/ for event devices, falling back to /dev/input/.
	 * @return Ordered map of id → display label.
	 */
	StringMap scanEventDevices();

	/**
	 * Populates the combo with detected sources plus sentinels.
	 * Uses id/display pairs so the stored value is stable across reboots.
	 * @param devices Ordered map from scanEventDevices().
	 */
	void populateSourcesList(const StringMap& devices);
};

} // namespace
