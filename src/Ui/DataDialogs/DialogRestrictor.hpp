/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogRestrictor.hpp
 * @since     Apr 30, 2023
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

#include "DialogFormHost.hpp"
#include "Storage/Restrictor.hpp"
#include "DialogRestrictorMap.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogRestrictor
 */
class DialogRestrictor : public DialogFormHost, public SingletonDialog<DialogRestrictor> {

	friend class Gtk::Builder;

public:

	inline static const string noRestrictor {"Select Restrictor"};

	virtual ~DialogRestrictor() = default;

	void load(DataMap& values)    noexcept override;
	void resetForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

	void setRotatorRunner(std::function<bool(const StringVector&, string&)> fn) noexcept { rotatorRunner = std::move(fn); }
	void setTestLive(std::function<bool()> fn) noexcept { testLive = std::move(fn); }

	/**
	 * Re-evaluates the way-icon test affordance: the three modes, the live
	 * test toggle, and whether the restrictor has any player mappings.
	 */
	void updateWaysTestState() noexcept;

protected:

	Gtk::ComboBox* comboBoxId = nullptr;

	Gtk::ToggleButton
		* williamsMode  = nullptr,
		* hasRestrictor = nullptr,
		* handleMouse   = nullptr;
	Gtk::SpinButton
		* speedOn  = nullptr,
		* speedOff = nullptr;
	Gtk::Entry* serialPort = nullptr;

	/// Restrictor ways icon map keyed by Ways enum value.
	std::unordered_map<Defaults::Ways, Gtk::FlowBoxChild*> waysIcons;

	/// Rotator CLI ways token to its internal Defaults::Ways type (also the way-icon glade ids).
	static const std::unordered_map<string, Defaults::Ways> wayTokens;

	/**
	 * @param way
	 * @return the rotator CLI token for a way (e.g. Ways::w2v -> "vertical2"), or empty if invalid.
	 */
	static const string& wayToToken(Defaults::Ways way) noexcept;

	/**
	 * Builds the positional rotator arguments to set player mappings to one way.
	 * @param mappings player/joystick number pairs to include in the test.
	 * @param way the requested way.
	 * @return flat token list, three per mapping: <player> <joystick> <ways>.
	 */
	static StringVector buildRotatorArgs(
		const std::vector<std::pair<string, string>>& mappings,
		Defaults::Ways way
	) noexcept;

	Gtk::ListStore* idListstore = nullptr;

	Gtk::Label* brief = nullptr;

	/// Section labels that gain a test hint when the test affordance is active.
	Gtk::Label
		* waysLabel     = nullptr,
		* mappingsLabel = nullptr;

	Gtk::Button* btnAddRestrictorMap = nullptr;

	/// Restrictor way-icon test group (one toggle per supported way).
	Gtk::FlowBox* flowboxWays = nullptr;
	/// Currently toggled way, or null when none.
	Gtk::FlowBoxChild* selectedWay = nullptr;
	/// Guards re-entrancy while a rotator run is in flight.
	bool rotatorRunning = false;

	/// Runs the rotator with positional args; true on a clean exit (set by MainWindow).
	std::function<bool(const StringVector&, string&)> rotatorRunner;
	/// True when a rotator test may run now: interactive, connected, fresh (set by MainWindow).
	std::function<bool()> testLive;

	DialogRestrictor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	void createSubItems(DataMap& values)               noexcept override;
	const string& getType()                        const noexcept override { return TYPE_RESTRICTOR; }
	Storage::Data* createData(Values& rawData) const noexcept override;

	void onEmpty()    noexcept override;
	void onSelected() noexcept override;

	/**
	 * Runs (or clears) the rotator test for the toggled way over all the
	 * restrictor's player mappings.
	 * @param child the activated way icon.
	 */
	void onWayActivated(Gtk::FlowBoxChild* child) noexcept;

};

} // namespace
