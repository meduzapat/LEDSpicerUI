/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileNode.cpp
 * @since     Feb 23, 2026
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

#include "FileNode.hpp"

using namespace LEDSpicerUI::Ui::Storage;

FileNode::FileNode(
	StringUMap& data,
	DirNode* parent,
	const string& collectionId,
	const vector<string>& childIds
) noexcept :
	Parent(data, collectionId, childIds),
	DirNode(parent)
{
	setProperty(FILENAME, fieldsData.count(FILENAME) ? fieldsData.at(FILENAME) : "");
	setProperty(PID, parent ? parent->getFsId() : "");
	setProperty(UID, "file_" + std::to_string(++fileCounter));
	fieldsData.erase(FILENAME);
}

const string FileNode::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		parent ? parent->getFsId() : "",
		getProperty(FILENAME)
	});
}

const string& FileNode::getName() const noexcept {
	return getProperty(FILENAME);
}

const string& FileNode::getFsId() const noexcept {
	return getProperty(UID);
}

const string FileNode::toXML() const noexcept {
	StringUMap attrs;
	for (const auto& [k, v] : fieldsData)
		if (shouldSerialize(k, v))
			attrs.emplace(k, v);
	string r(XMLHelper::xmlHeader(string(getXmlTag()), attrs));
	r += xmlBody();
	Defaults::reduceTab();
	r += XMLHelper::xmlFooter();
	return r;
}
