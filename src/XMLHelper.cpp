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
	rootInfo.attributes = processNode(root);

	rootInfo.version = valueOf(rootInfo.attributes, "version", "");
	rootInfo.type    = valueOf(rootInfo.attributes, "type",    "");

	// Validate version.
	if (rootInfo.version.empty() or rootInfo.version != PACKAGE_DATA_VERSION)
		throw Message("Invalid data file version, needed " PACKAGE_DATA_VERSION);

	// Validate type.
	if (not expectedType.empty() and rootInfo.type != expectedType)
		throw Message("Invalid data file type, needed " + expectedType);
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

StringUMap XMLHelper::getSettings() const noexcept {
	return rootInfo.attributes;
}

tinyxml2::XMLElement* XMLHelper::getRoot() const noexcept {
	return root;
}

const XMLHelper::RootInfo& XMLHelper::getRootInfo() const noexcept {
	return rootInfo;
}

void XMLHelper::checkAttributes(
	const StringVector& attributeList,
	const StringUMap& subjects,
	const string& place
) {
	for (const string& attribute : attributeList)
		if (subjects.find(attribute) == subjects.end())
			throw Message("Missing attribute '" + attribute + "' inside " + place);
}

const string& XMLHelper::valueOf(
	const StringUMap& values,
	const string& value
) noexcept {
	return (values.find(value) != values.end() ? values.at(value) : emptyString);
}

string XMLHelper::valueOf(
	const StringUMap& values,
	const string& value,
	string defaultValue
) noexcept {
	return (values.find(value) != values.end() ? values.at(value) : defaultValue);
}

string XMLHelper::xmlHeader(
	const string& type,
	const StringUMap& attrs
) noexcept {
	string r;
	r  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	r += "<!-- " DEFAULT_MESSAGE " -->\n";
	r += "<" PACKAGE_DATA_NAME "\n";
	Defaults::increaseTab();
	r += Defaults::tab() + "version=\"" PACKAGE_DATA_VERSION "\"\n";
	r += Defaults::tab() + "type=\"" + type + "\"\n";
	for (const auto& [k, v] : attrs)
		r += Defaults::tab() + k + "=\"" + v + "\"\n";
	Defaults::reduceTab();
	r += ">\n";
	Defaults::increaseTab();
	return r;
}

string XMLHelper::xmlSection(
	const string& tag,
	const string& content,
	const StringUMap& attrs
) noexcept {
	if (content.empty()) return "";
	string r(Defaults::tab() + "<" + tag);
	for (const auto& [k, v] : attrs)
		r += " " + k + "=\"" + v + "\"";
	r += ">\n";
	Defaults::increaseTab();
	r += content;
	Defaults::reduceTab();
	r += Defaults::tab() + "</" + tag + ">\n";
	return r;
}

string XMLHelper::xmlFooter() noexcept {
	return "</" PACKAGE_DATA_NAME ">\n";
}

string XMLHelper::toXML(const StringUMap& values) noexcept {
	string r;
	for (const auto& v : values)
		r += Defaults::tab() + v.first + "=\"" + v.second + "\"\n";
	return r;
}

StringUMapVector& XMLHelper::getData(const string& dataName) noexcept {
	return extractedData[dataName];
}

string XMLHelper::cleanError(const string& error) noexcept {
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
