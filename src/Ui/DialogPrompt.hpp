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
	 * Opens the structured name generator. Tabs and option lists are filtered
	 * by the calling element's current type.
	 * @return Composed element name, or empty on cancel.
	 */
	string askElementName(const string& elementTypeId, Gtk::Window* parent = nullptr) noexcept;

	/**
	 * Builds a Player Control element name (e.g. "P1_JOYSTICK1_8WAYS").
	 * Returns empty when player or type is missing, or when type requires an
	 * index and index is empty.
	 * Index is ignored when type is START or COIN.
	 * waysSuffix is appended verbatim ("4WAYS", "vertical2WAYS"…) when non-empty.
	 */
	static string buildPlayerControlName(
		const string& player,
		const string& type,
		const string& index,
		const string& waysSuffix
	) noexcept {
		if (player.empty() or type.empty()) return emptyString;
		string name {"P" + player + "_" + type};
		if (type == TYPE_START or type == TYPE_COIN) return name;
		if (index.empty()) return emptyString;
		name += index;
		if (not waysSuffix.empty()) name += "_" + waysSuffix;
		return name;
	}

	/**
	 * Builds a Cabinet Item element name (e.g. "FLOOR" or "FLOOR1").
	 * Returns empty when category is empty. Index is appended verbatim.
	 */
	static string buildCabinetItemName(
		const string& category,
		const string& index
	) noexcept {
		if (category.empty()) return emptyString;
		return category + index;
	}

protected:

	enum class Section : uint8_t { DirName, GroupName, ElementName };

	/// Pretty label + emit id for the Player Control TYPE combo.
	struct PlayerType {
		string id;
		string label;
	};

	/// Pretty label + emit id for the Cabinet Item Category combo.
	struct CabinetCategory {
		string id;
		string label;
	};

	/// Per-element-type rules driving page visibility and combo populations.
	struct Filter {
		bool       playerTab          = false;
		bool       cabinetTab         = false;
		StringUSet playerTypes;       /// empty → all
		bool       playerWaysAllowed  = false;
		StringUSet cabinetCategories; /// empty → all
	};

	/// Logic tokens — kept in one place so behaviour comparisons match the catalogs.
	static inline const string
		PAGE_PLAYER    {"player"},
		PAGE_CABINET   {"cabinet"},
		TYPE_START     {"START"},
		TYPE_COIN      {"COIN"},
		TYPE_JOYSTICK  {"JOYSTICK"},
		ICON_INVALID   {"dialog-question-symbolic"};

	static const vector<PlayerType>                 playerTypes;
	static const vector<CabinetCategory>            cabinetCategories;
	static const std::unordered_map<string, Filter> filters;

	Gtk::Box
		* boxName    = nullptr,
		* boxGroup   = nullptr,
		* boxElement = nullptr;

	Gtk::Entry* entryName = nullptr;

	Gtk::ComboBoxText
		* comboGN1                = nullptr,
		* comboGN2                = nullptr,
		* comboPlayer             = nullptr,
		* comboPlayerType         = nullptr,
		* comboPlayerIndex        = nullptr,
		* comboPlayerWays         = nullptr,
		* comboCabinetCategory    = nullptr,
		* comboCabinetIndex       = nullptr;

	Gtk::Stack*         stack         = nullptr;
	Gtk::StackSwitcher* stackSwitcher = nullptr;

	Gtk::Label
		* labelPlayerIndex   = nullptr,
		* labelPlayerWays    = nullptr,
		* previewLabel       = nullptr;

	Gtk::Image* previewIcon = nullptr;

	Gtk::Button* btnApply = nullptr;

	Filter currentFilter;

	DialogPrompt(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	/**
	 * Hides every section, shows the requested one, sets the title, runs modally,
	 * and returns the composed string (empty on cancel).
	 */
	int runFor(Section section, Gtk::Window* parent) noexcept;

	/// Applies the per-element-type filter to tabs and combo populations.
	void applyFilter(const string& elementTypeId) noexcept;

	/// Show/hide Index and Ways rows on the Player page per the current TYPE pick.
	void updatePlayerControlVisibility() noexcept;

	/// Rebuild preview text + icon and gate the Apply button.
	void updatePreview() noexcept;

	/// Compose the preview string from the active page's widgets.
	string buildPreview() const noexcept;
};

} // namespace
