/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.hpp
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

#include "CollectionHandler.hpp"
#include "Link.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Actor
 * Represents a single actor inside an Animation file.
 * The actor type (Filler, Pulse, Audio, ...) is stored in TYPE; the active
 * field set depends on the type's AnimationInfo flags.
 * Collection ID is scoped to its parent Animation via PID.
 * Extends Link to hold a pointer to its Group, enabling cascade deletion
 * when the group is removed.
 */
class Actor : public Link {

public:

	Actor(Values& data, Data* group, const string& ownerId) noexcept;

	virtual ~Actor() = default;

	string createUniqueId()     const noexcept override;
	string createPrettyName()   const noexcept override;
	string createTooltip()      const noexcept override;
	const string& getXmlTag()   const noexcept override { return TYPE_ACTOR; }
	const string& getCssClass() const noexcept override { return CSS_ACTOR_BOX_BUTTON; }

	CollectionHandler* getCollectionHandler() const noexcept override;

protected:

	const string& getPrimaryKey() const noexcept override { return TYPE; }

private:

	/// Counter for stable UID generation.
	inline static size_t actorCounter = 0;

};

} // namespace
