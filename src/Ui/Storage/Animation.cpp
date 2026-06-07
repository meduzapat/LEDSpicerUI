/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Animation.cpp
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

#include "Animation.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Animation::Animation(Values& data, DirNode* parent) noexcept :
	Parent(data, vector<string>{COLLECTION_ACTORS}),
	DirNode(getProperties(), parent, getValue(FILENAME))
{
	registerDependency(COLLECTION_GROUPS, COLLECTION_ACTORS);
}

string Animation::createUniqueId() const noexcept {
	return Defaults::createCommonUniqueId({
		not isAtRoot() ? parent->getFsId() : emptyString,
		getName()
	});
}

string Animation::createPrettyName() const noexcept {
	return getName();
}

string Animation::createTooltip() const noexcept {
	const auto actors {getChild(COLLECTION_ACTORS)};
	const auto count {actors ? actors->getSize() : 0};
	return "Animation with " + std::to_string(count) + " actor" + (count == 1 ? "" : "s");
}

string Animation::toXML() const noexcept {
	string xml {XMLHelper::xmlHeader(TYPE_ANIMATION)};
	for (auto btn : *getChild(COLLECTION_ACTORS))
		xml += btn->getData()->toXML();
	Defaults::reduceTab();
	xml += XMLHelper::xmlFooter();
	return xml;
}
