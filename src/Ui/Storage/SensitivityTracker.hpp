/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SensitivityTracker.hpp
 * @since     Apr 17, 2026
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

#include <gtkmm/widget.h>
#include <vector>
#include <algorithm>
#include <cstddef>

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::SensitivityTracker
 *
 * Mixin that tracks registered widgets and keeps their sensitivity in sync
 * with the collection size. Consumers call refreshSensitiveWidgets() from
 * their add() / remove() / create() methods.
 */
class SensitivityTracker {

public:

	virtual ~SensitivityTracker() = default;

	/**
	 * Registers a widget whose sensitivity mirrors this collection's size.
	 * Evaluated immediately and on every add() / remove() / create().
	 * @param widget   Widget to control.
	 * @param minCount Minimum item count required for sensitivity (default 1).
	 */
	void registerSensitivity(Gtk::Widget* widget, size_t minCount = 1) noexcept {
		sensitiveWidgets.push_back({widget, minCount});
		refreshSensitiveWidgets();
	}

	/// Removes a sensitivity binding.
	void releaseSensitive(Gtk::Widget* widget) noexcept {
		auto it{std::find_if(
			sensitiveWidgets.begin(),
			sensitiveWidgets.end(),
			[widget](const SensitivityBinding& b) {
				return b.widget == widget;
			}
		)};
		if (it != sensitiveWidgets.end()) sensitiveWidgets.erase(it);
	}

protected:

	/// Binds a widget's sensitivity to this collection's size.
	struct SensitivityBinding {
		Gtk::Widget* widget;
		size_t       minCount = 1;
	};

	/// Widgets whose sensitivity mirrors this collection's size.
	std::vector<SensitivityBinding> sensitiveWidgets;

	/// Updates sensitivity for all registered widgets.
	void refreshSensitiveWidgets() noexcept {
		for (auto& b : sensitiveWidgets)
			b.widget->set_sensitive(getSize() >= b.minCount);
	}

	/// Implemented by the owning collection.
	virtual size_t getSize() const noexcept = 0;

};

} // namespace
