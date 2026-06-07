/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogPrompt.hpp
 * @since     Jun 6, 2026
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

#include "GladeDialog.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * Shared single-line prompt dialog for short utility inputs.
 * One Glade dialog hosts every section; consumers call an intent-typed
 * ask* method and receive the composed string (empty on cancel).
 */
class DialogPrompt final : public GladeDialog<DialogPrompt> {

	friend class Gtk::Builder;

public:

	virtual ~DialogPrompt() = default;

	/**
	 * @param current Pre-fill text for the entry.
	 * @param parent  Transient parent; falls back to the main window when null.
	 * @return Sanitized filename, or empty on cancel.
	 */
	string askDirName(const string& current, Gtk::Window* parent = nullptr) noexcept;

	/**
	 * @return Composed group name (GN1.id + GN2.id), or empty on cancel.
	 */
	string askGroupName(Gtk::Window* parent = nullptr) noexcept;

	/**
	 * @return Composed element name (EN1.id + EN2.id [+ "_" + EN3.id + EN4.id]),
	 *         or empty on cancel.
	 */
	string askElementName(Gtk::Window* parent = nullptr) noexcept;

protected:

	enum class Section : uint8_t { DirName, GroupName, ElementName };

	Gtk::Box
		* boxName    = nullptr,
		* boxGroup   = nullptr,
		* boxElement = nullptr;

	Gtk::Entry* entryName = nullptr;

	Gtk::ComboBoxText
		* comboGN1 = nullptr,
		* comboGN2 = nullptr,
		* comboEN1 = nullptr,
		* comboEN2 = nullptr,
		* comboEN3 = nullptr,
		* comboEN4 = nullptr;

	Gtk::Button* btnApply = nullptr;

	DialogPrompt(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	/**
	 * Hides every section, shows the requested one, sets the title, runs modally,
	 * and returns the composed string (empty on cancel).
	 */
	int runFor(Section section, Gtk::Window* parent) noexcept;
};

} // namespace
