/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.hpp
 * @since     Mar 13, 2023
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
#include "DialogPrompt.hpp"
#include "Storage/Element.hpp"
#include "Storage/Group.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogElement
 */
class DialogElement: public DialogForm, public SingletonDialog<DialogElement> {

	friend class Gtk::Builder;

public:

	/// Tab numbers for elements LED modes.
	enum tabIndex : uint8_t {RGB, Strip, sRGB, Single, mRGB, TAB_COUNT};

	/// Bitmask with all tabs, used to clear the form.
	static constexpr uint8_t ALL_TAB_IDX = (1 << TAB_COUNT) - 1;

	/// Maximum and minimum columns for the pin display.
	static constexpr const int
		MAX_COLUMNS = 20,
		MIN_COLUMNS = 10;

	virtual ~DialogElement() {
		switchPageConnection.disconnect();
	};

	void load(DataMap& values)    noexcept override;
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

	/// Exposes the inherited edit entry so a parent dialog can drive an in-place edit.
	using DialogForm::onEditClicked;

	/**
	 * Sets the current number of pins.
	 * @param newSize
	 */
	void changeNumberOfPins(const uint16_t newSize) noexcept;

	/**
	 * Process the hardware conversion.
	 * @param fromType
	 * @param toType
	 * @param newPins
	 */
	void handleLayoutChange(
		const Defaults::DeviceInfo& oldInfo,
		const Defaults::DeviceInfo& newInfo
	) noexcept;

	/**
	 * Redraw the pin box.
	 */
	void drawPins() noexcept;

protected:

	string defaultRGBFormat;

	Gtk::SpinButton
		* timeOn = nullptr,
		* pin    = nullptr, /// Single color / actuator.
		* pinR   = nullptr,
		* pinG   = nullptr,
		* pinB   = nullptr;

	Gtk::Entry
		* elementName   = nullptr,
		* positionsMRGB = nullptr, /// Multi RGB.
		* positionRGB   = nullptr, /// Continuous RGB.
		* sizeStrip     = nullptr, /// LED Strip.
		* positionStrip = nullptr;

	/// Number of pins provided by the current hardware.
	uint16_t numberOfPins = 0;

	/// The tabs to select the different connection modes.
	Gtk::Notebook* notebookDeviceConnections = nullptr;

	Gtk::ToggleButton* actuator        = nullptr;
	Gtk::Button*       btnDefaultColor = nullptr;
	Gtk::FlowBox*      pinsBox         = nullptr;

	/// Only used by the UI.
	Gtk::ComboBox* elementType = nullptr;

	Gtk::Scale* brightness = nullptr;

	Gtk::ComboBox
		* comboBoxDevices  = nullptr, /// Device name.
		* comboBoxRGBRGB   = nullptr, /// Continuous RGB.
		* comboBoxRGBStrip = nullptr, /// LED Strip.
		* comboBoxRGBMRGB  = nullptr; /// Multi RGB.

	/// Connection for the notebook page switch, needed to disconnect and avoid errors.
	sigc::connection switchPageConnection;

	DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void clearFormConditinal(uint8_t flags) noexcept;

	const string& getType() const noexcept override { return TYPE_ELEMENT; }

	Storage::Data* createData(Values& rawData) const noexcept override;

	void addButtons(Storage::BoxButton& boxButton) noexcept override;

	/**
	 * Refresh the list of connectors.
	 * @param pinsUsage an array of pins with color, element name that owns it.
	 */
	void findConnectorTypes(vector<std::pair<string, string>>& pinsUsage) noexcept;

	/**
	 * Calculates the number of columns for the pin display.
	 * @param size
	 */
	void findLargestDivisor(uint16_t size) noexcept;

	void drawPinsRGB(vector<Gtk::Label*>& labels) noexcept;

	void drawPins(vector<Gtk::Label*>& labels) noexcept;

	void findElementByPin(uint16_t finder, std::unordered_set<Storage::BoxButton*>& elementsFound) noexcept;

	void onSwitchPage(Gtk::Widget*, uint pageNum) noexcept;

	/**
	 * Greys out element-type rows the current widget state forbids.
	 * If the user's current pick is no longer allowed, re-detects.
	 * User-facing orchestrator — call from every UI trigger that affects validity.
	 */
	void refreshElementTypeOptions() noexcept;

	/** True when the given type id is compatible with the current widget state. */
	bool isElementTypeAllowed(const string& id) const noexcept;

	/** True when the current combobox selection is compatible with the widgets. */
	bool validateElementType() const noexcept;

	/** Best-fit type id for the current widget state and name. */
	string detectElementType() const noexcept;

	void onCloneClicked(Storage::BoxButton& boxButton) noexcept override;

};

} // namespace
