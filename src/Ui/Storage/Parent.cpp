/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Parent.cpp
 * @since     Mar 23, 2026
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

#include "Parent.hpp"

using namespace LEDSpicerUI::Ui::Storage;

Parent::Parent(
	StringUMap& data,
	const vector<string>& childCollectionIds
) :
	Data(data)
{
	for (const auto& id : childCollectionIds) {
		children.emplace(id, BoxButtonCollection{});
		if (children.size() == 1) primaryChild = &children.at(id);
	}
}

Parent::~Parent() {
	for (auto& [watched, target] : dependencyRegistry)
		watched->release(target);
}

BoxButtonCollection* Parent::getChild(const string& collectionId) noexcept {
	auto it{children.find(collectionId)};
	return (it != children.end()) ? &it->second : nullptr;
}

const BoxButtonCollection* Parent::getChild(const string& collectionId) const noexcept {
	auto it{children.find(collectionId)};
	return (it != children.end()) ? &it->second : nullptr;
}

StringBoxButtonCollectionUMap& Parent::getChildren() noexcept {
	return children;
}

const StringBoxButtonCollectionUMap& Parent::getChildren() const noexcept {
	return children;
}

void Parent::registerDependency(
	const string& watchedCollection,
	const string& targetFamily
) noexcept {
	auto ch {CollectionHandler::getInstance(watchedCollection)};
	auto cf {&children.at(targetFamily)};
	ch->registerDependency(cf);
	dependencyRegistry.emplace_back(ch, cf);
}

string Parent::xmlBody() const noexcept {
	string r;
	for (const auto& [id, collection] : children) {
		for (const auto btn : collection) {
			Defaults::increaseTab();
			r += btn->getData()->toXML();
			Defaults::reduceTab();
		}
	}
	return r;
}
