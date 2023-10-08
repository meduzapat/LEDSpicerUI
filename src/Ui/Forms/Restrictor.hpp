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

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Form::Restrictor
 * A class that adds functionality for Restrictors form.
 */
class Restrictor : public Form {

public:

	struct RestrictorFields {
		Gtk::ComboBox
			* comboBoxRestrictors = nullptr,
			* comboBoxId          = nullptr;
		Gtk::ComboBoxText
			* player   = nullptr,
			* joystick = nullptr;
		Gtk::ToggleButton
			* williamsMode  = nullptr,
			* hasRestrictor = nullptr,
			* handleMouse   = nullptr;
		Gtk::SpinButton
			* speedOn  = nullptr,
			* speedOff = nullptr;
		Gtk::ListStore
			* restrictorsListstore = nullptr,
			* idListstore          = nullptr;
		unordered_map<Defaults::Ways, Gtk::FlowBoxChild*> waysIcons;
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

	virtual void resetForm(Modes mode);

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode);

	virtual const string getCssClass() const;

	static bool isDual(const string& name);

	static string getMaps(const string name, const string id, vector<unordered_map<string, string>> data);

protected:

	/**
	 * This combobox is shared between all the ID Restrictors, I cannot share the model
	 */
	static RestrictorFields* fields;

	/**
	 * Keeps control of duplicated Restrictors.
	 */
	static CollectionHandler* restrictors;


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
