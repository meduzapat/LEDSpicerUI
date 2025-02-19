/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogElement.h
 * @since     Mar 13, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)
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

#ifndef UI_DIALOGELEMENT_HPP_
#define UI_DIALOGELEMENT_HPP_ 1

	const int MAX_COLUMNS = 20;
	const int MIN_COLUMNS = 10;

#define elementHandler Storage::CollectionHandler::getInstance(COLLECTION_ELEMENT)

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogElement
 */
class DialogElement: public DialogForm {

	friend class Gtk::Builder;

public:

	/// Tab numbers for elements LED modes.
	enum class tabIndex : uint8_t {RGB, Strip, sRGB, Single, mRGB};

	DialogElement() = delete;

	virtual ~DialogElement() = default;

	/**
	 * Instantiate an object of its class.
	 * @param builder
	 * @param gladeID
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Return an instance of this class.
	 * @return
	 */
	static DialogElement* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

	/**
	 * Sets the current number of pins.
	 * @param newSize
	 */
	void changeNumberOfPins(const uint16_t newSize);

	/**
	 * Set special elements rules based on device.
	 * @param isMono
	 * @param canRGB
	 * @param canStrip
	 */
	void setRules(bool isMono, bool canRGB, bool canStrip);

	/**
	 * Redraw the pin box.
	 */
	void drawPins();

protected:

	enum tabBit : uint8_t {
		RGB    = 1 << static_cast<uint8_t>(tabIndex::RGB),
		Strip  = 1 << static_cast<uint8_t>(tabIndex::Strip),
		sRGB   = 1 << static_cast<uint8_t>(tabIndex::sRGB),
		Single = 1 << static_cast<uint8_t>(tabIndex::Single),
		mRGB   = 1 << static_cast<uint8_t>(tabIndex::mRGB)
	};

	/// Self instance.
	static DialogElement* instance;

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
		// Continuous RGB.
		* comboBoxRGBRGB   = nullptr,
		// LED Strip.
		* comboBoxRGBStrip = nullptr,
		// Multi RGB
		* comboBoxRGBMRGB  = nullptr;

	bool
		isMono   = false,
		canRGB   = false,
		canStrip = false;

	DialogElement(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void clearFormConditinal(uint8_t flags);

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	void addButtons(Storage::BoxButton* boxButton) override;

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

	void onSwitchPage(Gtk::Widget* page, guint page_num);

	void findElementByPin(uint16_t finder, unordered_set<Storage::BoxButton*>& elementsFound);
};

} /* namespace */

#endif /* UI_DIALOGELEMENT_HPP_ */
