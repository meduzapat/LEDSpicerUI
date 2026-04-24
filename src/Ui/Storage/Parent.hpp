/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Parent.hpp
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

#include "CollectionHandler.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Parent
 *
 * Mixin for Data subclasses that own one or more child BoxButtonCollections.
 * Children are keyed by collection ID (e.g. COLLECTION_ELEMENT) and injected
 * at construction time. Provides uniform access and iteration over all child
 * collections without knowledge of concrete subclass layout.
 */
class Parent : public Data {

public:

	/**
	 * Constructs a Parent with pre built child collections.
	 *
	 * @param data Serializable field data forwarded to Data.
	 * @param collectionId the collection id forwarded to Data.
	 * @param childCollectionIds a lit of collection ID to create the children.
	 */
	Parent(
		StringUMap& data,
		const vector<string>& childCollectionIds
	);

	virtual ~Parent();

	/**
	 * @param collectionId Collection key (e.g. COLLECTION_ELEMENT).
	 * @return Returns the child collection for the given ID, or nullptr if not found.
	 */
	BoxButtonCollection* getChild(const string& collectionId) noexcept;
	const BoxButtonCollection* getChild(const string& collectionId) const noexcept;

	/**
	 * @return Returns the primary child collection.
	 */
	BoxButtonCollection* getPrimaryChild() const noexcept { return primaryChild; }

	/**
	 * @return Returns the full children map.
	 */
	StringBoxButtonCollectionUMap& getChildren() noexcept;
	const StringBoxButtonCollectionUMap& getChildren() const noexcept;

	/// Iteration support.
	auto begin()           noexcept { return children.begin();  }
	auto end()             noexcept { return children.end();    }
	auto begin()     const noexcept { return children.begin();  }
	auto end()       const noexcept { return children.end();    }
	auto cbegin()    const noexcept { return children.cbegin(); }
	auto cend()      const noexcept { return children.cend();   }

	/**
	 * @return Returns the total number of child items in the primary child.
	 */
	virtual size_t getSize() const noexcept { return primaryChild->getSize(); }

protected:

	/// Child collections keyed by collection ID.
	StringBoxButtonCollectionUMap children;

	/// pointer to the primary child, parent without children is considered invalid.
	BoxButtonCollection* primaryChild{nullptr};

	/// List of dependencies acting over children, watchedCollection → targetCollection
	vector<std::pair<string, string>> dependencyRegistry;

	/**
	 * Registers a dependency between a watched collection and a child collection.
	 * @param watchedCollection The collection ID to watch for changes.
	 * @param targetCollection The child collection ID to apply the dependency to.
	 */
	void registerDependency(
		const string& watchedCollection,
		const string& targetCollection
	) noexcept;

	string xmlBody() const noexcept override;
};

} // namespace
