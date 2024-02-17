/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogProfile.hpp
 * @since     Feb 14, 2023
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

#include "DialogForm.hpp"
#include "Storage/Profile.hpp"

#ifndef UI_DIALOGPROFILE_HPP_
#define UI_DIALOGPROFILE_HPP_ 1

#define profileCollectionHandler Storage::CollectionHandler::getInstance(COLLECTION_PROFILES)

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DialogProfile
 * Dialog to create or edit profiles.
 */
class DialogProfile: public DialogForm {

	friend class Gtk::Builder;

public:

	using Selectors = LEDSpicerUI::Ui::Storage::Profile::Selectors;

	DialogProfile() = delete;

	virtual ~DialogProfile() = default;

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
	static DialogProfile* getInstance();

	void load(XMLHelper* values) override;

	void clearForm() override;

	void isValid() const override;

	void storeData() override;

	void retrieveData() override;

	const string createUniqueId() const override;

protected:

	/// Self instance.
	static DialogProfile* instance;

	/// Pointer to the entry for profile name input.
	Gtk::Entry*  inputProfileName          = nullptr;

	/// Pointer to the button for profile background color.
	Gtk::Button* btnProfileBackgroundColor = nullptr;

	/// Pointer to the box where different types of elements are displayed.
	OrdenableFlowBox
		/// The box where always on elements are displayed.
		* boxProfileAlwaysOnElements = nullptr,
		/// The box where always on groups are displayed.
		* boxProfileAlwaysOnGroups   = nullptr,
		/// The box where animations are displayed.
		* boxProfileAnimations       = nullptr,
		/// The box where inputs are displayed.
		* boxProfileInputs           = nullptr,
		/// The box where start transitions are displayed.
		* boxProfileStartTransitions = nullptr,
		/// The box where end transitions are displayed.
		* boxProfileEndTransitions   = nullptr;

	/// Pointer to the buttons.
	Gtk::Button
		/// Always on elements selector button.
		* btnProfilesAddElements = nullptr,
		/// Always on group selector.
		* btnProfilesAddGroups = nullptr,
		/// Animations selector.
		* btnProfilesAddAnimations = nullptr,
		/// Inputs selector.
		* btnProfilesAddInputs = nullptr,
		/// Start transition selector.
		* btnProfilesAddStartTransitions = nullptr,
		/// End transition selector.
		* btnProfilesAddEndTransitions = nullptr;

	DialogProfile(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* getData(unordered_map<string, string>& rawData) override;

	/**
	 * Prepares the Dialgo selector for the selector.
	 * @param selector
	 */
	void prepareSelect(Selectors selector) const;
};

} /* namespace */

#endif /* UI_DIALOGPROFILE_HPP_ */
