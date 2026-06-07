/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ElementObserverMock.hpp
 * @since     Jun 8, 2026
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

#include "Storage/ElementObserver.hpp"

#pragma once

namespace LEDSpicerUI::Test::Mocks {

using namespace LEDSpicerUI::Ui::Storage;

class MockElementObserver : public ElementObserver {

public:

	MockElementObserver() = default;
	virtual ~MockElementObserver() override = default;

	// Overrides
	void onAdded(Element*) noexcept override {
		m_onAddedCalled = true;
	}

	void onRemoved(Element*) noexcept override {
		m_onRemovedCalled = true;
	}

	void onChanged(Element*) noexcept override {
		m_onChangedCalled = true;
	}

	// Getters for testing
	bool isOnAddedCalled() const noexcept { return m_onAddedCalled; }
	bool isOnRemovedCalled() const noexcept { return m_onRemovedCalled; }
	bool isOnChangedCalled() const noexcept { return m_onChangedCalled; }

	// Helper to reset the mock state between test cases
	void reset() noexcept {
		m_onAddedCalled = false;
		m_onRemovedCalled = false;
		m_onChangedCalled = false;
	}

private:
	bool m_onAddedCalled{false};
	bool m_onRemovedCalled{false};
	bool m_onChangedCalled{false};
};

} // namespace

