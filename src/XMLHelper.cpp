/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
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

#include "XMLHelper.hpp"

using namespace LEDSpicerUI;

XMLHelper::XMLHelper(const string& fileName, const string& fileType) {

	if (LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
		throw Message("Unable to read the file " + fileName + " " + string(ErrorStr()));

	root = RootElement();

	// Foreign files skip validation.
	if (fileType == XML_FILE_PLAIN) return;

	if (not root or std::strcmp(root->Name(), PACKAGE_DATA_NAME))
		throw Message("Unknown or invalid data file");

	processRootNode(fileType);
}

void XMLHelper::processRootNode(const string& expectedType) {

	rootInfo = processNode(root);

	// Validate version.
	if (rootInfo.getValue("version") != PACKAGE_DATA_VERSION)
		throw Message("Invalid data file version, needed " PACKAGE_DATA_VERSION);

	// Validate type.
	if (not expectedType.empty() and rootInfo.getValue("type") != expectedType)
		throw Message("Invalid data file type, needed " + expectedType);
}

Values XMLHelper::processNode(const tinyxml2::XMLElement* node) {

	Values groupValues;

	const tinyxml2::XMLAttribute* pAttrib = node->FirstAttribute();

	while (pAttrib) {
		const string value {pAttrib->Value()};
		groupValues.setValue(pAttrib->Name(), value);
		pAttrib = pAttrib->Next();
	}

	return groupValues;
}

Values XMLHelper::processNode(const string& nodeElement) const {

	tinyxml2::XMLElement* node {root->FirstChildElement(nodeElement.c_str())};
	if (not node)
		throw Message("Missing " + nodeElement + " section.");

	return processNode(node);
}

void XMLHelper::checkAttributes(
	const StringVector& attributeList,
	const Values& subjects,
	const string& place
) {
	for (const string& attribute : attributeList)
		if (not subjects.isSet(attribute))
			throw Message("Missing attribute '" + attribute + "' inside " + place);
}

string XMLHelper::xmlHeader(const string& type, const Values& attrs) noexcept {

	string r;
	r  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	r += "<!-- " DEFAULT_MESSAGE " -->\n";
	r += "<" PACKAGE_DATA_NAME "\n";
	Defaults::increaseTab();
	r += Defaults::tab() + "version=\"" PACKAGE_DATA_VERSION "\"\n";
	r += Defaults::tab() + "type=\"" + Defaults::titleCase(type) + "\"\n";
	for (const auto& [k, v] : attrs)
		r += Defaults::tab() + k + "=\"" + Defaults::escapeXmlValue(v) + "\"\n";
	Defaults::reduceTab();
	r += ">\n";
	Defaults::increaseTab();
	return r;
}

string XMLHelper::xmlSection(
	const string& tag,
	const string& content,
	const Values& attrs
) noexcept {

	if (content.empty()) return "";
	string r(Defaults::tab() + "<" + tag);
	for (const auto& [k, v] : attrs)
		r += " " + k + "=\"" + Defaults::escapeXmlValue(v) + "\"";
	r += ">\n";
	Defaults::increaseTab();
	// Fixes indentation for multi-line content, adds a tab if the line is not empty.
	for (size_t pos = 0; pos < content.size(); ) {
		size_t nl = content.find('\n', pos);
		if (nl == string::npos) nl = content.size() - 1;
		const string line(content, pos, nl - pos + 1);
		if (line.find_first_not_of(" \t\r\n") != string::npos)
			r += '\t';
		r += line;
		pos = nl + 1;
	}
	Defaults::reduceTab();
	r += Defaults::tab() + "</" + tag + ">\n";
	return r;
}

string XMLHelper::xmlFooter() noexcept {
	return "</" PACKAGE_DATA_NAME ">\n";
}

string XMLHelper::toXML(const Values& values) noexcept {
	string r;
	for (const auto& v : values)
		r += Defaults::tab() + v.first + "=\"" + Defaults::escapeXmlValue(v.second) + "\"\n";
	return r;
}

string XMLHelper::cleanError(const string& error) noexcept {

	// ex: Unable to read the file /xxx/yyy/zzzz.xml
	// Error=XML_ERROR_MISMATCHED_ELEMENT ErrorID=14 (0xe)
	// Line number=369: XMLElement name=map
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
