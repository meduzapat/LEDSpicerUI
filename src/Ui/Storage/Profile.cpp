/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Sep 25, 2023
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

#include "Profile.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Profile::Profile(Values& data, DirNode* parent) noexcept :
	Parent(data, vector<string>{
		COLLECTION_PROFILE_ELEMENTS,
		COLLECTION_PROFILE_GROUPS,
		COLLECTION_PROFILE_INPUTS,
		COLLECTION_PROFILE_ANIMATIONS
	}),
	DirNode(getProperties(), parent, getValue(FILENAME))
{
	registerDependency(COLLECTION_ELEMENTS,   COLLECTION_PROFILE_ELEMENTS);
	registerDependency(COLLECTION_GROUPS,     COLLECTION_PROFILE_GROUPS);
	registerDependency(COLLECTION_INPUTS,     COLLECTION_PROFILE_INPUTS);
	registerDependency(COLLECTION_ANIMATIONS, COLLECTION_PROFILE_ANIMATIONS);
}

string Profile::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		not isAtRoot() ? parent->getFsId() : emptyString,
		getName()
	});
}

string Profile::createPrettyName() const noexcept {
	return getName();
}

string Profile::toXML() const noexcept {

	Values attrs;
	attrs.setValue(BACKGROUND_COLOR, getValue(BACKGROUND_COLOR));

	string xml {XMLHelper::xmlHeader(TYPE_PROFILE, attrs)};

	// Bare name attrs.
	const auto emit = [this](const string& tag, const string& collectionKey) {
		string inner;
		for (auto btn : *getChild(collectionKey))
			inner += btn->getData()->toXML();
		return XMLHelper::xmlSection(tag, inner);
	};

	// Full path attrs.
	const auto emitPathRefs = [this](
		const string& tag,
		const string& collectionKey,
		const string& itemTag,
		const string& sourceCollectionId
	) {
		auto* ch {CollectionHandler::getInstance(sourceCollectionId)};
		string inner;
		for (auto btn : *getChild(collectionKey)) {
			auto* target {ch->get(btn->getData()->createUniqueId())};
			auto* node   {dynamic_cast<const DirNode*>(target)};
			if (not node) continue;
			inner += Defaults::tab() + "<" + itemTag + " name=\"" + node->getFullPath() + "\"/>\n";
		}
		return XMLHelper::xmlSection(tag, inner);
	};

	xml += emit("alwaysOnElements", COLLECTION_PROFILE_ELEMENTS);
	xml += emit("alwaysOnGroups",   COLLECTION_PROFILE_GROUPS);
	xml += emitPathRefs("animations", COLLECTION_PROFILE_ANIMATIONS, TYPE_ANIMATION, COLLECTION_ANIMATIONS);
	xml += emitPathRefs("inputs",     COLLECTION_PROFILE_INPUTS,     TYPE_INPUT,     COLLECTION_INPUTS);

	Defaults::reduceTab();
	xml += XMLHelper::xmlFooter();
	return xml;
}
