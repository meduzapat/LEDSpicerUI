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

#include "DialogFormHost.hpp"
#include "DialogInputMap.hpp"
#include "Storage/InputSource.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInputSource
 * Dialog to add/edit event sources (hardware input devices) for multi-source inputs.
 * When running locally, scans /dev/input/ for event devices and populates a combo.
 * Selecting "Other" or running in portable mode falls back to a manual text entry.
 * Both the combo and entry resolve to a single source value via resolvedSource().
 */
class DialogInputSource : public DialogFormHost, public SingletonDialog<DialogInputSource> {

	friend class Gtk::Builder;

public:

	/// Linux event device directories.
	static constexpr const char* DEV_INPUT       = "/dev/input/";
	static constexpr const char* DEV_INPUT_BY_ID = "/dev/input/by-id/";
	static constexpr const char* SYS_CLASS_INPUT = "/sys/class/input/";

	virtual ~DialogInputSource();

	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

	/**
	 * Ensures a sourceless InputSource exists for single-source inputs.
	 * Creates one silently if absent, then wires DialogInputMap to it.
	 * Safe to call repeatedly, idempotent if sourceless source already exists.
	 */
	void createPhantomSource();

protected:

	/// Input type selector, shared with DialogInput.
	Gtk::ComboBox* comboBoxInputSelectInput = nullptr;

	/// Controls map-add button accessibility.
	Gtk::Button* btnAddMap = nullptr;

	DialogInputSource(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;
	string_view getType() const override;
	Storage::Data* createData(StringUMap& rawData) override;

private:

	/**
	 * Resolves the current source value from combo or manual entry.
	 * @return The resolved source string.
	 */
	Glib::ustring resolvedSource() const;

	/**
	 * Populates the source combo with detected devices plus sentinels.
	 * @param devices Ordered map of id → display label from scanEventDevices().
	 */
	void populateSourcesList(const StringMap& devices);

	/**
	 * Resolves a device entry to its kernel human-readable name via sysfs.
	 * @param  byIdName Entry from /dev/input/by-id/ or a plain eventX name.
	 * @return Kernel name string, or the original name on any failure.
	 */
	static string readDeviceName(const string& byIdName);

	/**
	 * Scans /dev/input/by-id/ for event devices, falling back to /dev/input/.
	 * @return Ordered map of id → display label.
	 */
	StringMap scanEventDevices();

	void onEmpty() override;

	void onSelected() override;
};

} // namespace
