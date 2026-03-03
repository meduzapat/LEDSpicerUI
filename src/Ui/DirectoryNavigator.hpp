/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryNavigator.hpp
 * @since     Feb 15, 2026
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

#include "Storage/BoxButtonCollection.hpp"
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::DirectoryNavigator
 * Manages directory-style navigation over a DirNode tree.
 * Each DirectoryEntry owns its level contents via BoxButtonCollection.
 * Reusable for Inputs, Profiles, and Animations.
 */
class DirectoryNavigator {

public:

	DirectoryNavigator() = delete;

};

} // namespace
