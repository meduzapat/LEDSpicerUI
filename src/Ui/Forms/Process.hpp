/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Process.hpp
 * @since     May 2, 2023
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

#ifndef PROCESS_HPP_
#define PROCESS_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::Process
 */
class Process: public Form {

public:

	struct Fields {
		Gtk::Entry
			* InputProcessName = nullptr,
			* InputSystemType  = nullptr,
			* InputRomPosition = nullptr;
	};

	Process() = delete;

	Process(unordered_map<string, string>& data);

	virtual ~Process();

	static void initialize(Fields& fields);

	virtual void resetForm(Modes mode);

	virtual void isValid(Modes mode);

	virtual void storeData(Modes mode);

	virtual void retrieveData(Modes mode);

	virtual void cancelData(Modes mode) {}

	virtual const string getCssClass() const;

	virtual const string createPrettyName() const;

	virtual const string createName() const;

protected:

	static Fields* fields;

	static CollectionHandler* processHandler;
};

} /* namespace */


#endif /* PROCESS_HPP_ */
