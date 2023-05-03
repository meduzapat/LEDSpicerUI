/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Form.hpp
 * @since     Feb 28, 2023
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

#include "Message.hpp"

#ifndef FORM_HPP_
#define FORM_HPP_ 1

namespace LEDSpicerUI::Ui::Forms {

/**
 * LEDSpicerUI::Ui::Forms::Form
 * Class to handle customized Form sections.
 * It represents a vertical box with possible fields to customize a form dialog, it also contains the data.
 */
class Form: public Gtk::VBox {

public:

	/**
	 * Posible states of the form,
	 */
	enum class Modes : uint8_t {
		ADD,  /// Adding new
		LOAD, /// Loading new
		EDIT  /// Editing existing
	};

	Form() = default;

	/**
	 * Creates an object pre-populated.
	 * @param data
	 */
	Form(unordered_map<string, string>& data);

	virtual ~Form() = default;

	/**
	 * Checks of the form fields are valid.
	 * @throw Message with error if something is wrong
	 */
	virtual void isValid(Modes mode) = 0;

	/**
	 * Read the fields and store the values.
	 */
	virtual void storeData(Modes mode) = 0;
	
	/**
	 * Retrieves data back to the fields.
	 */
	virtual void retrieveData(Modes mode) = 0;

	/**
	 * Cancel changes.
	 */
	virtual void cancelData(Modes mode) = 0;

	/**
	 * Provides a css class that identifies the object and the data.
	 */
	virtual const string getCssClass() const = 0;

	/**
	 * Create a pretty name for the form entry.
	 * @return
	 */
	virtual const string createPrettyName() const = 0;

	/**
	 * Create a name for the form entry.
	 * @return
	 */
	virtual const string createName() const;

	/**
	 * @brief Converts the form data into XML string.
	 * @return The XML string representation of the box element.
	 */
	virtual string toXML();

	/**
	 * Returns a value using the key name.
	 * @param key
	 * @return
	 */
	string getValue(const string& key) const;

	/**
	 * Allows the change of an element.
	 * @param key
	 * @param value
	 */
	void setValue(const string& key, const string& value);

	/**
	 * Returns a list of stored values.
	 * @return
	 */
	const unordered_map<string, string>* getValues() const;

	/**
	 * @return true if the allows editions.
	 */
	virtual bool canEdit() const;

	/**
	 * @return true if the allows deletions.
	 */
	virtual bool canDelete() const;

protected:

	/// Data pairs.
	unordered_map<string, string> fieldsData;
};

} /* namespace */

#endif /* FORM_HPP_ */
