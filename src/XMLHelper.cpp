/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
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

#include "XMLHelper.hpp"

using namespace LEDSpicerUI;

XMLHelper::XMLHelper(const string& fileName, const string& fileType) {

	if (LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
		throw Message("Unable to read the file " + fileName + " " + string(ErrorStr()));

	root = RootElement();
	if (fileType != XML_FILE_FOREIGN) {
		if (not root or std::strcmp(root->Name(), PACKAGE_DATA_NAME))
			throw Message("Unknown or invalid data file");

		if (not root->Attribute("version") or std::strcmp(root->Attribute("version"), PACKAGE_DATA_VERSION))
			throw Message("Invalid data file version, needed " PACKAGE_DATA_VERSION);

		if (not root->Attribute("type") or fileType != root->Attribute("type"))
			throw Message("Invalid data file type, needed " + fileType);
	}
}

unordered_map<string, string> XMLHelper::processNode(tinyxml2::XMLElement* nodeElement) {

	unordered_map<string, string> groupValues;

	const tinyxml2::XMLAttribute* pAttrib = nodeElement->FirstAttribute();

	while (pAttrib) {
		string value = pAttrib->Value();
		groupValues.emplace(pAttrib->Name(), value);
		pAttrib = pAttrib->Next();
	}

	return groupValues;
}

unordered_map<string, string> XMLHelper::processNode(const string& nodeElement) {

	tinyxml2::XMLElement* node = root->FirstChildElement(nodeElement.c_str());
	if (not node)
		throw Message("Missing " + nodeElement + " section.");

	return processNode(node);
}

tinyxml2::XMLElement* XMLHelper::getRoot() {
	return root;
}

void XMLHelper::checkAttributes(
	const vector<string>& attributeList,
	const unordered_map<string, string>& subjects,
	const string& place)
{
	for (string attribute : attributeList)
		if (not subjects.count(attribute))
			throw Message("Missing attribute '" + attribute + "' inside " + place);
}

string XMLHelper::valueOf(const unordered_map<string, string>& values, const string& value, string def) {
	return (values.count(value) ? values.at(value) : def);
}

string XMLHelper::toXML(const unordered_map<string, string>& values) {
	string r;
	for (const auto& v : values)
		r += Defaults::tab() + v.first + "=\"" + v.second + "\"\n";
	return r;
}

