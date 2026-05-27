/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AnimationFile.cpp
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

#include "AnimationFile.hpp"

using namespace LEDSpicerUI::Config;

AnimationFile::AnimationFile(const string& filePath, Ui::Storage::DirectoryEntry* parent) :
	ProjectFile(filePath, "Animation", parent)
{
	const string relPath {parent ? parent->getFullPath() : emptyString};
	const string baseId  {Defaults::createCommonUniqueId({relPath, filename})};

	Values animation;
	animation.setValue(FILENAME, filename);
	animation.setValue(PATH_BASE, baseId);

	// Walk every <actor> child of the root.
	ValueVector actors;
	tinyxml2::XMLElement* actorNode = getRoot()->FirstChildElement("actor");
	for (; actorNode; actorNode = actorNode->NextSiblingElement("actor")) {
		Values actor {processNode(actorNode)};
		actors.push_back(std::move(actor));
	}

	extractedData.emplace(
		Defaults::createCommonUniqueId({baseId, COLLECTION_ACTORS}),
		std::move(actors)
	);

	ValueVector animationVec;
	animationVec.push_back(std::move(animation));
	extractedData.emplace(
		Defaults::createCommonUniqueId({relPath, COLLECTION_ANIMATIONS}),
		std::move(animationVec)
	);
}

void AnimationFile::save(const Ui::Storage::Animation& animation, const string& filePath) {
	saveFile(filePath, animation.toXML());
}
