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

//string Profile::xmlBody() const noexcept {
//	string r;
//	const auto emit = [&](const string& tag, const string& key) {
//		const auto& col = children.at(key);
//		if (col.getSize() == 0) return;
//		r += XMLHelper::xmlSection(tag, [&]{
//			string s;
//			for (const auto& e : col)
//				s += e->getData()->toXML();
//			return s;
//		}());
//	};
//	emit("alwaysOnElements", COLLECTION_PROFILE_ELEMENTS);
//	emit("alwaysOnGroups",   COLLECTION_PROFILE_GROUPS);
//	emit("inputs",           COLLECTION_PROFILE_INPUTS);
//	emit("animations",       COLLECTION_PROFILE_ANIMATIONS);
//	return r;
//}
