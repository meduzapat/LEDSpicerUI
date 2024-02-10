/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.hpp
 * @since     Apr 30, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

#include "Storage/Restrictor.hpp"

#ifndef UI_DIALOGRESTRICTOR_HPP_
#define UI_DIALOGRESTRICTOR_HPP_ 1

#define restrictorsHandler Storage::CollectionHandler::getInstance(COLLECTION_RESTRICTORS)

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogRestrictor
 */
class DialogRestrictor: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogRestrictor() = delete;

	virtual ~DialogRestrictor() = default;

	/**
	 * Instanciate an object of its class.
	 * @param builder
	 * @param gladeID
	 */
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);

	/**
	 * Return an instance of this class.
	 * @return
	 */
	static DialogRestrictor* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

protected:

	/// Self instance.
	static DialogRestrictor* instance;

	Gtk::ComboBox
		* comboBoxRestrictors = nullptr,
		* comboBoxId          = nullptr;
	Gtk::ToggleButton
		* williamsMode  = nullptr,
		* hasRestrictor = nullptr,
		* handleMouse   = nullptr;
	Gtk::SpinButton
		* speedOn  = nullptr,
		* speedOff = nullptr;
	Gtk::Entry
		* serialPort = nullptr;

	// icons.
	unordered_map<Defaults::Ways, Gtk::FlowBoxChild*> waysIcons;

	Gtk::ListStore
		* restrictorsListstore = nullptr,
		* idListstore          = nullptr;

	string previousName;

	DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	/**
	 * Reset all fields except devices id.
	 */
	void clearFormOthers();

	void markRestrictorUsed();
};

} /* namespace */

#endif /* UI_DIALOGRESTRICTOR_HPP_ */
