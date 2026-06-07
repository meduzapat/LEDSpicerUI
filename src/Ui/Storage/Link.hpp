/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Link.hpp
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

#include "Data.hpp"
#include "Defaults.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Link
 * Links a Data object as a value, optionally carrying its own extra fieldsData.
 * key and type are stable references owned by the caller (e.g. SelectionRequest).
 * Only the Data pointer changes via setLink().
 */
class Link : public Data {

public:

	struct LinkField {

		/// Widget types rendered for this field.
		enum class Widget : uint8_t { COLOR_PICKER, COMBOBOX };

		const string
			/// fieldsData key on the Link.
			key,
			/// Human label displayed next to the widget.
			label,
			/// Value used when the Link has no stored value yet.
			defaultValue;

		/// Widget to use.
		const Widget widgetType;
	};

	/**
	 * @param data       Extra fields owned by this Link (e.g. DefaultColor).
	 * @param linkKey    The key used to identify the linked Data (e.g. "name").
	 * @param linkType   Semantic type used in XML output (e.g. "element").
	 * @param linkFields Fields editable via DialogLinkEditor; {} if none.
	 * @param link       The Data object this Link points to.
	 */
	Link(
		Values&                  data,
		const string&            linkKey,
		const string&            linkType,
		const vector<LinkField>& linkFields,
		const Data*              link
	) noexcept : Data(data), linkKey(linkKey), linkType(linkType), linkFields(linkFields), link(link) {}

	Link(Link&& other) noexcept :
		Data(std::move(other)),
		linkKey(other.linkKey),
		linkType(other.linkType),
		linkFields(other.linkFields),
		link(other.link)
	{}

	virtual ~Link() = default;

	bool operator==(const Data& other) const noexcept override;

	const string& getCssClass() const noexcept override;
	const string& getXmlTag()   const noexcept override;
	string createUniqueId()     const noexcept override;
	string createPrettyName()   const noexcept override;
	string createTooltip()      const noexcept override;
	string toXML()              const noexcept override;

	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }

	/**
	 * @return The collection from where Links can be picked.
	 */
	CollectionHandler* getCollectionHandlerSource() const noexcept {
		return link->getCollectionHandler();
	}

	/**
	 * If key matches linkKey returns link->getPrimaryId(); otherwise delegates to Data.
	 * @param key
	 * @return
	 */
	const string& getValue(const string& key) const noexcept override;

	/**
	 * If key matches linkKey returns link->getPrimaryId(); otherwise delegates to Data.
	 * @param key
	 * @param defaultValue
	 * @return
	 */
	string getValue(const string& key, const string& defaultValue) const noexcept override;

	/**
	 * Silently ignores linkKey; all other keys delegate to Data.
	 * @param key
	 * @param value
	 */
	void setValue(const string& key, const string& value) noexcept override;

	/**
	 * Replaces the target Data pointer.
	 * @param newLink The new Data object to point to.
	 */
	void setLink(const Data* newLink) noexcept { link = newLink; }

	const vector<LinkField>& getLinkFields() const noexcept { return linkFields; }

protected:

	/// Stable ref to the key that identifies the linked Data in XML (e.g. "name").
	const string& linkKey;

	/// Stable ref to the semantic type used in XML output (e.g. "element").
	const string& linkType;

	/// Stable ref to the fields editable via DialogLinkEditor.
	const vector<LinkField>& linkFields;

	/// The Data object this Link points to.
	const Data* link;
};

} // namespace
