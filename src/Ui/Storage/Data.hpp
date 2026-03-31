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

#include "Message.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

// Forward declaration
class CollectionHandler;

/**
 * LEDSpicerUI::Ui::Storage::Data
 *
 * Data will store information to be moved around.
 */
class Data {

public:

	Data() = delete;

	/**
	 * Creates an object pre-populated.
	 * @param data Iten's data.
	 * @param collection Id to use.
	 */
	Data(
		StringUMap& data,
		const string& collectionId
	) noexcept :
		fieldsData(std::move(data)),
		collectionId(std::move(collectionId)) {}

	/**
	 * Compares two Data objects for equality.
	 * @param other The other Data object to compare with.
	 * @return True if the objects are equal, false otherwise.
	 */
	virtual bool operator==(const Data& other) const noexcept {return this == &other;}

	/**
	 * Unregisters from collection if registered.
	 */
	virtual ~Data();

	/**
	 * @returns a CSS class that identifies the object and the data.
	 */
	virtual constexpr string_view getCssClass() const noexcept abstract;

	/**
	 * Creates a human readable name for the form.
	 *
	 * @return pretty name for the form.
	 */
	virtual const string createPrettyName() const noexcept;

	/**
	 * Creates a tooltip.
	 *
	 * @return the tooltip text.
	 */
	virtual const string createTooltip() const noexcept { return ""; }

	/**
	 * Creates a unique ID for the form.
	 *
	 * @return unique ID out of current data.
	 */
	virtual const string createUniqueId() const noexcept;

	/**
	 * Alias of value(primary key)
	 * @return a string with the primary key value or ""
	 */
	const string getPrimaryValue() const noexcept;

	/**
	 * Removes a value by key name.
	 * @param key
	 */
	virtual void unSet(const string& key) noexcept;


	/**
	 * Returns a value using the key name.
	 *
	 * @param key
	 * @return value or empty string if not found.
	 */
	virtual const string& getValue(const string& key) const noexcept;

	/**
	 * Returns a value using the key name.
	 *
	 * @param key
	 * @param defaultValue the default value if the key is not found.
	 * @return value or default.
	 */
	virtual string getValue(const string& key, const string& defaultValue) const noexcept;

	/**
	 * Allows the change of an internal value.
	 * @param key
	 * @param value
	 */
	virtual void setValue(const string& key, const string& value) noexcept;

	/**
	 * Creates a copy of the internal data but adding
	 * a copy number to the primary key value
	 *
	 * @return a copy of the values with a new primary key.
	 */
	StringUMap copyValues() const noexcept;

	/**
	 * @return a list of stored values
	 */
	const StringUMap* getValues() const noexcept;

	/**
	 * Replace values from a map.
	 *
	 * @param values
	 */
	void setValues(const StringUMap& values) noexcept;

	/**
	 * Clears all serializable fields.
	 */
	virtual void wipe() noexcept;

	/**
	 * Serializes this object to XML.
	 * Default emits flat attributes using getXmlTag().
	 * Subclasses override for children or conditional field exclusions.
	 * @return XML string.
	 */
	virtual const string toXML() const noexcept;

	/**
	 * Returns the XML tag name for this data type.
	 * Used by toXML() helpers to emit the correct element name.
	 * @return XML tag string, e.g. "device", "element", "group".
	 */
	virtual string_view getXmlTag() const noexcept abstract;

	/**
	 * Sets a property value.
	 *
	 * @param key Property name.
	 * @param value Property value.
	 */
	void setProperty(const string& key, const string& value) noexcept;

	/**
	 * Gets a property value.
	 * @param key Property name.
	 * @return Property value or empty string if not found.
	 */
	const string& getProperty(const string& key) const noexcept;

	/**
	 * Gets a property value.
	 * @param key Property name.
	 * @param def Default value if not found.
	 * @return Property value or default.
	 */
	string getProperty(const string& key, const string& defaultProperty) const noexcept;

	/**
	 * Checks if a property exists.
	 * @param key Property name.
	 * @return True if property exists.
	 */
	bool hasProperty(const string& key) const noexcept;

	/**
	 * Removes a property by key name.
	 * @param key
	 */
	void removeProperty(const string& key) noexcept;

	/**
	 * @return All properties.
	 */
	const StringUMap& getProperties() const noexcept;

	/**
	 * @return an empty data object.
	 */
	static StringUMap& createEmptyData() noexcept;

	virtual void setUp() noexcept {}

	virtual void tearDown() noexcept {}

	/**
	 * @return the CollectionHandler this item registers itself into.
	 */
	CollectionHandler* getCollectionHandler() const noexcept;

	/**
	 * Registers this item into its collection.
	 * No-op if already registered or if createUniqueId() is empty.
	 * Normally called when the object is realized, but can be called manually if needed.
	 */
	void registerToCollection() noexcept;

	/**
	 * Unregisters this item from its collection.
	 * No-op if not registered or if createUniqueId() is empty.
	 */
	void unregisterFromCollection() noexcept;

protected:

	/// Data pairs.
	StringUMap fieldsData;

	/// Extra property with important information.
	StringUMap properties;

	/// The Id of the collection to use
	const string collectionId;

	/**
	 * Returns the field name used as the primary identifier for this data type.
	 *
	 * @return Field name to use as primary key (default: NAME).
	 */
	virtual const string getPrimaryKey() const noexcept { return NAME; }

	/**
	 * Handles auto-registration and re-keying after a primary key field changes.
	 * @param oldId The unique ID before the change, empty if not yet registered.
	 */
	void handleRegistration(const string& oldId) noexcept;

	/**
	 * Organizes data vertically or horizontally based on the number of elements.
	 * @param ignored
	 * @param data
	 * @return
	 */
	static string valuesXML(
		const StringUSet& ignored,
		const StringUMap& data
	) noexcept;

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
		const StringUSet& ignored,
		bool empty
	) noexcept;

	/**
	 * @param node
	 * @return
	 */
	static string createClosingXML(const string& node) noexcept;

};

using StringDataMap = std::map<string, Data*>;

} // namespace
