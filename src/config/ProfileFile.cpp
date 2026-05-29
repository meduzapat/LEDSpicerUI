/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProfileFile.cpp
 * @since     May 2026
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

#include "ProfileFile.hpp"

using namespace LEDSpicerUI::Config;

ProfileFile::ProfileFile(const string& filePath, Ui::Storage::DirectoryEntry* parent) :
	ProjectFile(filePath, "Profile", parent)
{
	const string relPath {parent ? parent->getFullPath() : emptyString};
	const string baseId  {Defaults::createCommonUniqueId({relPath, filename})};

	Values profile {rootInfo.copyValues()};
	profile.setValue(FILENAME, filename);
	profile.setValue(PATH_BASE, baseId);
	if (not profile.isSet(BACKGROUND_COLOR))
		profile.setValue(BACKGROUND_COLOR, DEFAULT_PROFILE_BACKGROUND_COLOR);

	// Walk a single named wrapper section and collect its inner elements into a vector.
	const auto extractSection = [this, &baseId](
		const string& wrapperTag,
		const string& itemTag,
		const string& collectionKey
	) {
		tinyxml2::XMLElement* wrapper = getRoot()->FirstChildElement(wrapperTag.c_str());
		if (not wrapper) return;
		ValueVector items;
		for (
			tinyxml2::XMLElement* node = wrapper->FirstChildElement(itemTag.c_str());
			node;
			node = node->NextSiblingElement(itemTag.c_str())
		) {
			items.push_back(processNode(node));
		}
		if (items.empty()) return;
		extractedData.emplace(
			Defaults::createCommonUniqueId({baseId, collectionKey}),
			std::move(items)
		);
	};

	extractSection("alwaysOnElements", TYPE_ELEMENT,   COLLECTION_PROFILE_ELEMENTS);
	extractSection("alwaysOnGroups",   TYPE_GROUP,     COLLECTION_PROFILE_GROUPS);
	extractSection("animations",       TYPE_ANIMATION, COLLECTION_PROFILE_ANIMATIONS);
	extractSection("inputs",           TYPE_INPUT,     COLLECTION_PROFILE_INPUTS);

	ValueVector profileVec;
	profileVec.push_back(std::move(profile));
	extractedData.emplace(
		Defaults::createCommonUniqueId({relPath, COLLECTION_PROFILES}),
		std::move(profileVec)
	);
}

void ProfileFile::save(const Ui::Storage::Profile& profile, const string& filePath) {
	saveFile(filePath, profile.toXML());
}
