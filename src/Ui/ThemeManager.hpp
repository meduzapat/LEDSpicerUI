/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ThemeManager.hpp
 * @since     Jun 2026
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

#include <glibmm.h>
#include <gtkmm.h>
#include <tinyxml2.h>
#include <iostream>
#include "config/Settings.hpp"

#pragma once

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::ThemeManager
 *
 * Singleton that discovers, loads, and switches themes at runtime.
 *
 * Each theme lives in PACKAGE_DATA_DIR/themes/<id>/ and provides:
 *   metadata.xml     — display name and description
 *   preview.png      — mandatory selector thumbnail
 *   dark/theme.css   — dark variant palette
 *   light/theme.css  — light variant palette
 *
 * CSS is loaded in two layers:
 *   1. style-base.css    at APPLICATION   priority (structural bones, no colors/images)
 *   2. theme/variant.css at APPLICATION+1 priority (colors, backgrounds, images)
 *
 * Falls back to the legacy style.css if themes are unavailable.
 */
class ThemeManager {

public:

	/// Intrinsic theme description parsed from metadata.xml.

	struct ThemeMetadata {
		string
			id,
			name,
			description; /// Shown as tooltip on the theme selector tile.
	};

	ThemeManager(const ThemeManager&)            = delete;
	ThemeManager& operator=(const ThemeManager&) = delete;

	static ThemeManager& getInstance() noexcept { return instance; }

	/**
	 * Enumerates bundled themes from the GResource bundle and loads their metadata.
	 * Called once from main() before the main window is created.
	 */
	void initialize() noexcept;

	/**
	 * Loads and applies style-base.css + the theme variant CSS to the default screen.
	 * Replaces any previously loaded theme providers.
	 * @param themeId  Directory name under themes/.
	 * @param style    ThemeStyle::Auto detects system dark-mode preference.
	 * @return true on success, false if CSS files could not be loaded.
	 */
	bool apply(const string& themeId, Config::Settings::ThemeStyle style) noexcept;

	/**
	 * @return All discovered themes, in discovery order.
	 */
	const vector<ThemeMetadata>& getThemes() const noexcept { return themes; }

	/**
	 * @return The id of the currently applied theme.
	 */
	const string& getCurrentThemeId() const noexcept { return currentThemeId; }

	/**
	 * @return The currently applied variant string ("dark" or "light").
	 */
	const string& getCurrentVariant() const noexcept { return currentVariant; }

private:

	ThemeManager() = default;

	static ThemeManager instance;

	string
		currentThemeId,
		currentVariant;

	vector<ThemeMetadata> themes;

	Glib::RefPtr<Gtk::CssProvider>
		baseProvider,
		themeProvider;

	/**
	 * @param style
	 * @return Returns "dark" or "light" for the given style, querying GTK for Auto.
	 */
	static string resolveVariant(Config::Settings::ThemeStyle style) noexcept;

	/**
	 * Parses the bundled themes/<id>/metadata.xml and appends to themes on success.
	 *
	 * @param resourcePrefix GResource prefix for the theme (e.g. /org/ledspicer/ui/themes/default/).
	 * @param themeId
	 */
	void parseMetadata(const string& resourcePrefix, const string& themeId) noexcept;

	/**
	 * Loads a CSS resource into a new provider on the default screen at the given priority.
	 *
	 * @param resourcePath GResource path to the CSS file (e.g. /org/ledspicer/ui/style-base.css).
	 * @param priority Priority to apply the provider at (e.g. GTK_STYLE_PROVIDER_PRIORITY_APPLICATION).
	 * @return The loaded provider, or nullptr if loading failed.
	 */
	static Glib::RefPtr<Gtk::CssProvider> loadCss(const string& resourcePath, guint priority) noexcept;

	/**
	 * Removes a provider from the default screen if non-null.
	 * @param provider
	 */
	static void removeProvider(Glib::RefPtr<Gtk::CssProvider>& provider) noexcept;
};

} // namespace
