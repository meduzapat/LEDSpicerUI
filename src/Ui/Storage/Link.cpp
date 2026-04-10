/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Link.cpp
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

#include "Link.hpp"

using namespace LEDSpicerUI::Ui::Storage;

bool Link::operator==(const Data& other) const noexcept {
	return Data::operator==(other) or linkInfo.link == &other;
}

string_view Link::getCssClass() const noexcept {
	return linkInfo.link ? linkInfo.link->getCssClass() : "";
}

const string Link::createPrettyName() const noexcept {
	return linkInfo.link ? linkInfo.link->createPrettyName() : "";
}

const string Link::createTooltip() const noexcept {
	return linkInfo.link ? linkInfo.link->createTooltip() : "";
}

const string Link::createUniqueId() const noexcept {
	return linkInfo.link ? linkInfo.link->createUniqueId() : "";
}

const string& Link::getValue(const string& key) const noexcept {
	if (key == linkInfo.key) {
		return linkInfo.link->getValue(getPrimaryKey());
	}
	return Data::getValue(key);
}

string Link::getValue(const string &key, const string &defaultValue) const noexcept {
	if (key == linkInfo.key) {
		return linkInfo.link->getValue(getPrimaryKey(), defaultValue);
	}
	return Data::getValue(key, defaultValue);
}

void Link::setValue(const string& key, const string& value) noexcept {
	// Ignore
	if (key == linkInfo.key) return;
	Data::setValue(key, value);
}

string_view Link::getXmlTag() const noexcept {
	return linkInfo.link ? linkInfo.link->getXmlTag() : "";
}

const string Link::toXML() const noexcept {
	StringUMap values{{linkInfo.key, linkInfo.link->createUniqueId()}};
	values.insert(begin(), end());
	return createOpeningXML(string(linkInfo.type), values, true);
}

void Link::setLinkData(const LinkData& newLinkData) noexcept {
	linkInfo = std::move(newLinkData);
}
