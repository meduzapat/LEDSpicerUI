/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Link.hpp
 * @since     Mar 23, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "Data.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Link
 * Class that links other Data classes as values.
 */
class Link : public Data {

	public:

	/**
	 * LEDSpicerUI::Ui::Storage::LinkData
	 * Holds the immutable data required for a Link connection.
	 */
	struct LinkData {
		string
			type,
			key;
		const Data* link;

		LinkData() = delete;

		LinkData(
			const string& t,
			const string& k,
			const Data* l
		) noexcept : type(t), key(k), link(l) {}

	};

	using Data::Data;

	/**
	 * Constructor initializing the Link with existing data and a LinkData struct.
	 * * @param data Reference to the underlying data map.
	 * @param linkData The struct containing type, key, and the Data pointer.
	 */
	Link(
		StringUMap& data,
		const LinkData& linkData
	) noexcept : Data(data, ""), linkInfo(linkData) {}

	/**
	 * Constructor initializing the Link with individual values.
	 * @param data Reference to the underlying data map.
	 * @param type The link type string.
	 * @param key The link key string.
	 * @param link Pointer to the Data object to link.
	 */
	Link(
		StringUMap& data,
		const string& type,
		const string& key,
		Data* link
	) noexcept : Data(data, ""), linkInfo(type, key, link) {}

	bool operator==(const Data& other) const noexcept override;

	Link(Link&& other) noexcept :
		Data(std::move(other)),
		linkInfo(std::move(other.linkInfo))
	{}

	virtual ~Link() = default;

	string_view getCssClass()       const noexcept override;
	const string createPrettyName() const noexcept override;
	const string createTooltip()    const noexcept override;
	const string createUniqueId()   const noexcept override;
	const string toXML()            const noexcept override;
	string_view getXmlTag()         const noexcept override;

	const string& getValue(const string& key)                      const noexcept override;
	string getValue(const string& key, const string& defaultValue) const noexcept override;
	void setValue(const string& key, const string& value)                noexcept override;

	/**
	 * Replaces the internal link information with a new LinkData struct.
	 * * @param newLinkData The new struct to apply to this Link.
	 */
	void setLinkData(const LinkData& newLinkData) noexcept;

protected:

	/// Struct containing the immutable type, key, and Data link pointer.
	LinkData linkInfo;
};

} // namespace
