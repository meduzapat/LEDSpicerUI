/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.hpp
 * @since     Feb 19, 2023
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

#include <tinyxml2.h>
#include "Message.hpp"

#ifndef XMLHELPER_HPP_
#define XMLHELPER_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR PACKAGE ".conf"
#define XML_FILE_FOREIGN ""

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::Ui::XMLHelper
 */
class XMLHelper : protected tinyxml2::XMLDocument {

public:

	/**
	 * Creates a new XMLHelper object and open the XML file.
	 *
	 * @param fileName
	 * @throws Error if the file is missing, the body is missing or the version is different.
	 */
	XMLHelper(const string& fileName, const string& fileType);

	/**
	 * Reads the attributes from a XML node.
	 *
	 * @param parentElement
	 * @param nodeName
	 * @return A map with the key pairs.
	 * @throws Error if an error happen.
	 */
	static unordered_map<string, string> processNode(tinyxml2::XMLElement* nodeElement);

	/**
	 * Reads the attributes from a node by its name on the root.
	 *
	 * @param nodeName The name of the node, note that only the first occurrence will be read.
	 * @return A map with the parameters in that node.
	 * @throws Error if node does not exist or an error happen.
	 */
	unordered_map<string, string> processNode(const string& nodeName);

	/**
	 * Returns a pointer to the root node.
	 * @return
	 */
	tinyxml2::XMLElement* getRoot();

	/**
	 * Checks if the map subject have the attributeList elements.
	 * This function ignores other elements.
	 *
	 * @param attributeList A list of attributes to check.
	 * @param subjects A map to check.
	 * @param node where will check.
	 * @throws Error if an attribute is missing.
	 */
	static void checkAttributes(
		const vector<string>& attributeList,
		const unordered_map<string, string>& subjects,
		const string& place
	);

	/**
	 * Explodes a string into chunks using a delimiter.
	 *
	 * @param text
	 * @param delimiter
	 * @param limit
	 * @return an array with the string chunks.
	 */
	static vector<string> explode(
		const string& text,
		const char delimiter,
		const size_t limit = 0
	);

	/**
	 * Merge an array into a string using a delimiter.
	 * @param values
	 * @param delimiter
	 * @return
	 */
	static string implode(const vector<string>& values, const char delimiter);

	/**
	 * Return the value from the map or empty string.
	 * @param values
	 * @param value
	 * @return
	 */
	static string valueOf(
		const unordered_map<string, string>& values,
		const string& value,
		string def = ""
	);

protected:

	/// Pointer to the root element.
	tinyxml2::XMLElement* root = nullptr;

};

} /* namespace LEDSpicerUI */

#endif /* XMLHELPER_HPP_ */
