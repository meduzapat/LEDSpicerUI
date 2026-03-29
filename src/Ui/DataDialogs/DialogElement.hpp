/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.h
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
	enum tabIndex : uint8_t {RGB, Strip, sRGB, Single, mRGB};

	static constexpr const int MAX_COLUMNS = 20;
	static constexpr const int MIN_COLUMNS = 10;

	virtual ~DialogElement() = default;

	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void clearForm() noexcept override;
	void isValid() const override;
	void storeData() noexcept override;
	void retrieveData() noexcept override;
	const string createUniqueId() const noexcept override;

	/**
	 * Sets the current number of pins.
	 * @param newSize
	 */
	void changeNumberOfPins(const uint16_t newSize);

	/**
	 * Redraw the pin box.
	 */
	void drawPins();

protected:

	string defaultRGBFormat;

	Gtk::Entry
		* elementName = nullptr,
	// Single color.
		* pin = nullptr,
	// RGB.
		* pinR = nullptr,
		* pinG = nullptr,
		* pinB = nullptr,
	// onTime.
		* timeOn = nullptr,
	// Multi RGB.
		* positionsMRGB = nullptr,
	// Continuous RGB.
		* positionRGB = nullptr,
	// LED Strip.
		* sizeStrip     = nullptr,
		* positionStrip = nullptr;

	/// Number of pins provided by the current hardware.
	uint16_t numberOfPins = 0;

	/// The tabs to select the different connection modes.
	Gtk::Notebook* notebookDeviceConnections;

	Gtk::ToggleButton* solenoid  = nullptr;
	Gtk::Button* btnDefaultColor = nullptr;
	Gtk::FlowBox* pinsBox        = nullptr;

	// Only used by the UI
	Gtk::ComboBoxText* elementType = nullptr;

	Gtk::Scale* brightness = nullptr;

	Gtk::ComboBox
		// Device name.
		* comboBoxDevices  = nullptr,
		// Continuous RGB.
		* comboBoxRGBRGB   = nullptr,
		// LED Strip.
		* comboBoxRGBStrip = nullptr,
		// Multi RGB
		* comboBoxRGBMRGB  = nullptr;

	DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void clearFormConditinal(uint8_t flags);

	string_view getType() const noexcept override;

	Storage::Data* createData(StringUMap& rawData) noexcept override;

	void addButtons(Storage::BoxButton& boxButton) override;

	/**
	 * Refresh the list of connectors.
	 * @param pinsUsage an array of pins with color, element name that owns it.
	 */
	void findConnectorTypes(vector<std::pair<string, string>>& pinsUsage);

	/**
	 * Calculates the number of columns.
	 * @param size
	 * @return
	 */
	void findLargestDivisor(uint16_t size);

	void drawPinsRGB(vector<Gtk::Label*>& labels);

	void drawPins(vector<Gtk::Label*>& labels);

	void findElementByPin(uint16_t finder, std::unordered_set<Storage::BoxButton*>& elementsFound);

	void onSwitchPage(Gtk::Widget*, uint pageNum);
};

} // namespace
