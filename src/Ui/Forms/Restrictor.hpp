/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Restrictor.hpp
 * @since     Apr 30, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include "Form.hpp"
#include "CollectionHandler.hpp"

#ifndef RESTRICTOR_HPP_
#define RESTRICTOR_HPP_ 1

#define PLAYER               "player"
#define JOYSTICK             "joystick"
#define US360_HAS_RESTRICTOR "hasRestrictor"
#define US360_USE_MOUSE      "handleMouse"
#define GZ49_WILLIAMS        "williams"
#define GZ40_SPEED_ON        "speedOn"
#define GZ40_SPEED_OFF       "speedOff"
#define GZ40_DEFAULT_SPEED 12

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Form::Restrictor
 * A class that adds functionality for Restrictors form.
 */
class Restrictor : public Form {

public:

	/// Taken from Restrictors.
	enum class Ways : uint8_t {invalid, w2, w2v, w4, w4x, w8, w16, w49, analog, mouse, rotary8, rotary12};

	struct RestrictorInfo {
		const string       name;
		const uint8_t      maxIds;
		const vector<Ways> ways;
	};

	struct RestrictorFields {
		Gtk::ComboBox* comboBoxRestrictors   = nullptr;
		Gtk::ComboBox* comboBoxId            = nullptr;
		Gtk::ComboBoxText* player            = nullptr;
		Gtk::ComboBoxText* joystick          = nullptr;
		Gtk::CheckButton* williamsMode       = nullptr;
		Gtk::CheckButton* hasRestrictor      = nullptr;
		Gtk::CheckButton* handleMouse        = nullptr;
		Gtk::SpinButton* speedOn             = nullptr;
		Gtk::SpinButton* speedOff            = nullptr;
		Gtk::ListStore* restrictorsListstore = nullptr;
		Gtk::ListStore* idListstore          = nullptr;
	};

	Restrictor() = delete;

	Restrictor(unordered_map<string, string>& data);

	virtual ~Restrictor();

	/**
	 * @brief Populates the given Restrictor store with available Restrictors.
	 * @param fields pointer to the Restrictor fields.
	 */
	static void initialize(RestrictorFields* fields);

	/**
	 * @param name
	 * @return returns true if the Restrictor can be used.
	 */
	static bool isAvailable(const string& name);

	virtual const string createPrettyName() const;

	virtual const string createName() const;

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode);

	virtual const string getCssClass() const;

protected:

	/**
	 * This combobox is shared between all the ID Restrictors, I cannot share the model
	 */
	static RestrictorFields* fields;

	/**
	 * Keeps control of duplicated Restrictors.
	 */
	static CollectionHandler* restrictors;

	/// A list of restrictor to their information.
	static const unordered_map<string, RestrictorInfo> restrictorsInfo;

	static bool isDual(const string& name);

	/**
	 * Due to the GPWiz40RotoX been able to use 2 players this replaces the collection is used.
	 * @param name
	 * @param id
	 * @return
	 */
	static bool isUsed(const string& name, const string& id);

	/**
	 * Populates the Combobox.
	 */
	void processIdComboBox();

	void markRestrictorUsed();

};

} /* namespace */

#endif /* RESTRICTOR_HPP_ */
