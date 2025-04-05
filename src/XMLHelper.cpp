/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
 * @since     Feb 19, 2023
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

StringUMap XMLHelper::processNode(const tinyxml2::XMLElement* node) {

	StringUMap groupValues;

	const tinyxml2::XMLAttribute* pAttrib = node->FirstAttribute();

	while (pAttrib) {
		const string value = pAttrib->Value();
		groupValues.emplace(pAttrib->Name(), value);
		pAttrib = pAttrib->Next();
	}

	return groupValues;
}

StringUMap XMLHelper::processNode(const string& nodeElement) {

	tinyxml2::XMLElement* node = root->FirstChildElement(nodeElement.c_str());
	if (not node)
		throw Message("Missing " + nodeElement + " section.");

	return processNode(node);
}

tinyxml2::XMLElement* XMLHelper::getRoot() const {
	return root;
}

void XMLHelper::checkAttributes(
	const StringVector& attributeList,
	const StringUMap& subjects,
	const string& place)
{
	for (const string& attribute : attributeList)
		if (subjects.find(attribute) == subjects.end())
			throw Message("Missing attribute '" + attribute + "' inside " + place);
}

string XMLHelper::valueOf(const StringUMap& values, const string& value, string def) {
	return (values.find(value) != values.end() ? values.at(value) : def);
}

string XMLHelper::toXML(const StringUMap& values) {
	string r;
	for (const auto& v : values)
		r += Defaults::tab() + v.first + "=\"" + v.second + "\"\n";
	return r;
}

StringUMapVector& XMLHelper::getData(const string& dataName) {
	return extractedData[dataName];
}

string XMLHelper::cleanError(const string& error) {
	// ex: Unable to read the file /xxx/yyy/zzzz.xml Error=XML_ERROR_MISMATCHED_ELEMENT ErrorID=14 (0xe) Line number=369: XMLElement name=map
	string result;
	size_t pos = error.find("Error=");
	if (pos == std::string::npos)
		return error;

	result = error.substr(0, pos);
	Defaults::trim(result);
	size_t endPos  = pos + 6;
	pos = error.find("ErrorID=", endPos);
	if (pos == std::string::npos)
		return result;

	result += "\nError: " + error.substr(endPos, pos - endPos);
	Defaults::trim(result);
	pos = error.find("Line number=", pos);
	if (pos == std::string::npos)
		return result;

	pos += 12;
	endPos = error.find(':', pos);
	if (endPos == std::string::npos)
		return result;

	result += "\nLine: " + error.substr(pos, endPos - pos);
	Defaults::trim(result);
	pos = error.find("XMLElement name=", pos);
	if (pos == std::string::npos)
		return result;

	result += "\nNode: " + error.substr(pos + 16);
	Defaults::trim(result);
	return result;
}
