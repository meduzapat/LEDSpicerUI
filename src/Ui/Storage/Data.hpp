/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Data.hpp
 * @since     Feb 28, 2023
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

#include "Values.hpp"

// This looks cool.
#define abstract = 0

#pragma once

namespace LEDSpicerUI::Ui::Storage {

// Forward declaration
class CollectionHandler;

/**
 * LEDSpicerUI::Ui::Storage::Data
 *
 * Data will store information to be moved around.
 */
class Data : public Values {

public:

	using Values::Values;

	Data(Values& other)  noexcept : Values(other) {}
	Data(Values&& other) noexcept : Values(std::move(other)) {}

	Data(Data&&) noexcept = default;
	Data& operator=(Data&&) noexcept = default;

	/**
	 * Compares two Data objects for equality.
	 * @param other The other Data object to compare with.
	 * @return True if the objects are equal, false otherwise.
	 */
	virtual bool operator==(const Data& other) const noexcept {return this == &other;}

	virtual ~Data() = default;

	/**
	 * @returns a CSS class that identifies the object and the data.
	 */
	virtual const string& getCssClass() const noexcept abstract;

	/**
	 * Creates a human readable name for the form.
	 *
	 * @return pretty name for the form.
	 */
	virtual string createPrettyName() const noexcept;

	/**
	 * Creates a tooltip.
	 *
	 * @return the tooltip text.
	 */
	virtual string createTooltip() const noexcept { return emptyString; }

	/**
	 * Creates a unique ID for the form.
	 *
	 * @return unique ID out of current data.
	 */
	virtual string createUniqueId() const noexcept;

	/**
	 * Alias of value(primary key)
	 * @return a string with the primary key value or ""
	 */
	const string& getPrimaryValue() const noexcept;

	/**
	 * Serializes this object to XML.
	 * Default emits flat attributes using getXmlTag().
	 * @return XML string.
	 */
	virtual string toXML() const noexcept;

	/**
	 * Returns the XML tag name for this data type.
	 * Used by toXML() helpers to emit the correct element name.
	 * @return XML tag string, e.g. "device", "element", "group".
	 */
	virtual const string& getXmlTag() const noexcept abstract;

	/**
	 * @return a reference to the internal properties map, allowing direct manipulation.
	 */
	Values& getProperties() noexcept { return properties; }

	/**
	 * @return a const reference to the internal properties map, allowing read-only access.
	 */
	const Values& getProperties() const noexcept { return properties; }

	StringUMap copyValues() const noexcept override;

	void wipe() noexcept override;

	void unSet(const string& key) noexcept override;

	virtual void setUp() noexcept {}

	virtual void tearDown() noexcept {}

	/**
	 * @return the CollectionHandler this item registers itself into.
	 */
	virtual CollectionHandler* getCollectionHandler() const noexcept abstract;

	/**
	 * Registers this item into its collection.
	 * No-op if already registered or if createUniqueId() is empty.
	 * Normally called when the object is realized, but can be called manually if needed.
	 */
	virtual void registerToCollection() noexcept;

	/**
	 * Unregisters this item from its collection.
	 * No-op if not registered or if createUniqueId() is empty.
	 */
	virtual void unregisterFromCollection() noexcept;

	/**
	 * Synchronizes the item's registration in its CollectionHandler after a primary key change.
	 * Call with the previous unique ID when the primary key has been changed manually.
	 * No-op when the ID is unchanged or when there is no handler.
	 * @param oldId The unique ID before the change.
	 */
	void syncRegistration(const string& oldId) noexcept;

protected:

	/// Extra property with important information.
	Values properties;

	/**
	 * Returns the field name used as the primary identifier for this data type.
	 *
	 * @return Field name to use as primary key (default: NAME).
	 */
	virtual const string& getPrimaryKey() const noexcept { return NAME; }

	/**
	 * Controls which fields are included in XML output.
	 * Override to exclude fields conditionally.
	 * Default: serialize all fields.
	 * @param key   Field name.
	 * @param value Field value.
	 * @return True to include in XML, false to exclude.
	 */
	virtual bool shouldSerialize(
		const string&,
		const string& value
	) const noexcept {
		return not value.empty();
	}

	/**
	 * @return Content between opening and closing tag.
	 */
	virtual string xmlBody() const noexcept { return ""; }

	/**
	 * Organizes data vertically or horizontally based on the number of elements.
	 * @param data
	 * @return
	 */
	static string valuesXML(const StringUMap& data) noexcept;

	/**
	 * Based on data it creates a single or multiple node.
	 * @param node
	 * @param data
	 * @param ignored
	 * @param empty
	 * @return
	 */
	static string createOpeningXML(
		const string& node,
		const StringUMap& data,
		bool empty
	) noexcept;

	/**
	 * @param node
	 * @return
	 */
	static string createClosingXML(const string& node) noexcept;

	static StringUMap& emptyData() noexcept {
		static StringUMap data;
		return data;
	}
};

} // namespace
