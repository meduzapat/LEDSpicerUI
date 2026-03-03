/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogInput.hpp
 * @since     Feb 14, 2023
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

#include "Storage/Input.hpp"
#include "DialogFileForm.hpp"
#include "DialogImport.hpp"
#include "config/InputFile.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogInput
 * Dialog to create and edit Inputs.
 */
class DialogInput : public DialogFileForm, public SingletonDialog<DialogInput> {

	friend class Gtk::Builder;

public:

	static constexpr const char* BOX_INPUTS = "BoxInputs";

	virtual ~DialogInput();

	void load(XMLHelper* values) override;
	Storage::CollectionHandler* getCollectionHandler() const override;
	void resetForm() override;
	void clearForm() override;
	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	const string createUniqueId() const override;

	/// Sets the working directory path for new input files.
	void setPath(const string& path);

protected:

	/// Import input dialog.
	DialogImport dialogImportInput;

	Gtk::ComboBoxText
		* comboBoxInputSelectInput = nullptr,
		* comboBoxInputSpeed       = nullptr;
	Gtk::Entry*      entryInputName   = nullptr;
	Gtk::Switch*     switchInputBlink = nullptr;
	Gtk::SpinButton* spinInputTimes   = nullptr;
	Gtk::Box
		* boxInputSourcesBox        = nullptr,
		* boxInputCreditsSettings   = nullptr;

	/// The box to display linked maps.
	OrdenableFlowBox* boxInputLinkedMaps = nullptr;

	DialogInput(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder);

	void createSubItems(XMLHelper* values) override;
	const string getType() const override;
	Storage::Data* createData(StringUMap& rawData) override;

};

} // namespace
