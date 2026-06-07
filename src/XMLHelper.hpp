/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.hpp
 * @since     Feb 19, 2023
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

#include <tinyxml2.h>
#include "Message.hpp"

#pragma once

#define XML_FILE_PLAIN ""
#define DEFAULT_MESSAGE "This is an auto-generated file by " PACKAGE_STRING "."

namespace LEDSpicerUI {

using LEDSpicerUI::Ui::Message;

/**
 * LEDSpicerUI::XMLHelper
 */
class XMLHelper : protected tinyxml2::XMLDocument {

public:

	/**
	 * Creates a new XMLHelper object and open the XML file.
	 *
	 * @param fileName The path to the XML file to load.
	 * @param fileType The expected type attribute of the root node (e.g., "Configuration").
	 * @throws Message if the file is missing or corrupted, the body is missing or the version is different.
	 */
	XMLHelper(const string& fileName, const string& fileType);

	/**
	 * Reads the attributes from a XML node.
	 *
	 * @param node
	 * @return A map with the key pairs.
	 * @throws Message if an error happen.
	 */
	static Values processNode(const tinyxml2::XMLElement* node);

	/**
	 * Reads the attributes from a node by its name on the root.
	 *
	 * @param nodeName The name of the node, note that only the first occurrence will be read.
	 * @return A map with the parameters in that node.
	 * @throws Message if node does not exist or an error happen.
	 */
	Values processNode(const string& nodeName) const;

	/**
	 * @return a pointer to the root node.
	 */
	tinyxml2::XMLElement* getRoot() const noexcept { return root; }

	/**
	 * @return Root node information.
	 */
	const Values& getRootInfo() const noexcept { return rootInfo; }

	/**
	 * Checks if the map subject have the attributeList elements.
	 * This function ignores other elements.
	 *
	 * @param attributeList A list of attributes to check.
	 * @param subjects to check.
	 * @param place where will check.
	 * @throws Message if an attribute is missing.
	 */
	static void checkAttributes(
		const StringVector& attributeList,
		const Values& subjects,
		const string& place
	);

	/**
	 * Generates a complete XML file header with closed root opening tag.
	 * @param type  File type attribute (Input, Animation, Profile, etc.)
	 * @param attrs Additional root attributes beyond version and type.
	 * @return Complete root opening including closing >.
	 */
	static string xmlHeader(const string& type, const Values& attrs = {}) noexcept;

	/**
	 * Wraps content in a named XML section.
	 * Skips output entirely if content is empty.
	 * @param tag     Element tag name.
	 * @param content Inner XML string.
	 * @param attrs   Optional attributes on the opening tag.
	 */
	static string xmlSection(
		const string&     tag,
		const string&     content,
		const Values& attrs = {}
	) noexcept;

	/**
	 * Generates XML root closing tag.
	 * @return Closing tag string.
	 */
	static string xmlFooter() noexcept;

	/**
	 * Converts a map into xml string.
	 * @param values
	 * @return
	 */
	static string toXML(const Values& values) noexcept;

	/**
	 * Data may or may not exists, in that case will be created empty.
	 * @param dataName
	 * @return The stored values for that collection.
	 */
	ValueVector& getData(const string& dataName) noexcept { return extractedData[dataName]; }

	/**
	 * @return The whole extracted information at loading.
	 */
	DataMap& getDataMap() noexcept { return extractedData; }

	/**
	 * Convert a XML error into human readable text.
	 *
	 * @param error A raw error message from XML.
	 * @return A text in a more human way to be displayed.
	 */
	static string cleanError(const string& error) noexcept;

protected:

	/// Pointer to the root element.
	tinyxml2::XMLElement* root = nullptr;

	/// Root node information (version, type, attributes).
	Values rootInfo;

	/// Populated by derived classes to store extracted XML data by section.
	DataMap extractedData;

	/**
	 * Processes root node and extracts common attributes.
	 * Validates type and version against PACKAGE_DATA_VERSION.
	 * @param expectedType Expected type attribute.
	 * @throws Message if type mismatch or invalid version.
	 */
	void processRootNode(const string& expectedType);

};

} // namespace
