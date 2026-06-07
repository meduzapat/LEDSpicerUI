/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogAnimation.hpp
 * @since     Feb 14, 2023
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

#include "Storage/Animation.hpp"
#include "DialogForm.hpp"
#include "DirectoryAware.hpp"
#include "DialogActor.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogAnimation
 *
 * File-level dialog for an Animation. Holds the filename entry and the
 * ordered collection of Actor children that make up the animation file;
 * actor type selection / per-type fields live in DialogActor.
 */
class DialogAnimation :
	public DialogForm,
	public DirectoryAware,
	public SingletonDialog<DialogAnimation>
{

	friend class Gtk::Builder;

public:

	virtual ~DialogAnimation() = default;

	void load(DataMap& values)    noexcept override;
	void clearForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

protected:

	Gtk::Entry* entryAnimationName = nullptr;

	DialogAnimation(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(DataMap& values)      noexcept override;
	const string& getType()             const noexcept override { return TYPE_ANIMATION; }

	Storage::Data* createData(Values& rawData) const noexcept override;

};

} // namespace
