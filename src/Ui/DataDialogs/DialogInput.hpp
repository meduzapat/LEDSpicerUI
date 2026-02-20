/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInput.hpp
 * @since     Feb 14, 2023
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

#include "config/InputFile.hpp"
#include "Storage/Input.hpp"
#include "DialogForm.hpp"
#include "DialogImport.hpp"
#include "DialogInputSource.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInput
 * Dialog to create and edit Input plugins.
 */
class DialogInput : public DialogForm {

	friend class Gtk::Builder;

public:

	static constexpr const char* BOX_INPUTS = "BoxInputs";

	DialogInput() = delete;

	virtual ~DialogInput() = default;

	static void initialize(Glib::RefPtr<Gtk::Builder> const& builder);
	static DialogInput* getInstance();
	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
	void clearForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

	void setPath(const string& path);

protected:

	static DialogInput* instance;

	/// Import input dialog.
	DialogImport dialogImportInput;

	Gtk::ComboBoxText
		* comboBoxInputSelectInput = nullptr,
		* comboBoxInputSpeed       = nullptr;
	Gtk::Entry*      entryInputName   = nullptr;
	Gtk::Switch*     switchInputBlink = nullptr;
	Gtk::SpinButton* spinInputTimes   = nullptr;
	Gtk::Button*     btnAddInputMap   = nullptr;
	Gtk::Box
		* boxLinkedElementsAndGroupsBox = nullptr,
		* boxInputSourcesBox               = nullptr,
		* boxInputCreditsSettings       = nullptr;

	/// The box to select mappings.
	OrdenableFlowBox
		* boxInputMaps        = nullptr,
		* boxInputLinkedMaps = nullptr;

	string currentPath;

	DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;

	const string getType() const override;

	Storage::Data* createData(StringUMap& rawData) override;

};

} // namespace
