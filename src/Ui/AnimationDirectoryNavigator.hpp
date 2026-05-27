/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AnimationDirectoryNavigator.hpp
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

#include "DirectoryNavigator.hpp"
#include "DataDialogs/DialogAnimation.hpp"
#include "DataDialogs/DialogActor.hpp"
#include "config/AnimationFile.hpp"
#include "DialogImport.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::AnimationDirectoryNavigator
 * Specialized navigator for Animation files.
 * Handles loading/saving and manages DialogAnimation.
 */
class AnimationDirectoryNavigator : public DirectoryNavigator {

public:

	AnimationDirectoryNavigator(const Glib::RefPtr<Gtk::Builder>& builder, Gtk::Window* parentWindow) noexcept;

	virtual ~AnimationDirectoryNavigator() = default;

	void clear() noexcept override;

protected:

	/// Import dialog for existing LEDSpicer animation files.
	DialogImport dialogImportAnimation;

	Gtk::Box* boxBreadcrumb = nullptr;

	Gtk::Button* btnHome = nullptr;

	/// Display box for the animations list at the active directory.
	OrdenableFlowBox* boxAnimations = nullptr;

	void wireDialogs() noexcept override;

	void extractData(const string& filePath, Storage::DirectoryEntry* parent) override;

	void saveItem(Storage::Data* item, const string& filePath) const noexcept override;

	const string& getSubDir() const noexcept override { return PATH_ANIMATION; }

};

} // namespace
