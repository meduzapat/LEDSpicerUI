/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.hpp
 * @since     Apr 30, 2023
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

#include "Storage/Restrictor.hpp"

#ifndef UI_DIALOGRESTRICTOR_HPP_
#define UI_DIALOGRESTRICTOR_HPP_ 1

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogRestrictor
 */
class DialogRestrictor: public DialogForm {

	friend class Gtk::Builder;

public:

	DialogRestrictor() = delete;

	virtual ~DialogRestrictor() = default;
	static void initialize(Glib::RefPtr<Gtk::Builder> const &builder);
	static DialogRestrictor* getInstance();
	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
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

	// Stores the restrictors Ways icons.
	unordered_map<Defaults::Ways, Gtk::FlowBoxChild*> waysIcons;

	Gtk::ListStore
		* restrictorsListstore = nullptr,
		* idListstore          = nullptr;

	Gtk::Label* briefRestrictor = nullptr;

	string previousName;

	DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);
	void createSubItems(XMLHelper* values) override;
	const string getType() const override;
	Storage::Data* createData(StringUMap& rawData) override;

	/**
	 * Destroys and creates the data again.
	 */
	void recreateData();

	/**
	 * Reset all fields except devices id.
	 */
	void clearForm();

	void markRestrictorUsed();
};

} /* namespace */

#endif /* UI_DIALOGRESTRICTOR_HPP_ */
