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

#include <iostream>

#include "config/Settings.hpp"
#include "XMLHelper.hpp"

#pragma once

#define THEME_TYPE   "Theme"
#define THEME_FORMAT "1.0"

namespace LEDSpicerUI::Ui {

/**
 * LEDSpicerUI::Ui::ThemeManager
 *
 * Singleton that discovers, loads, and switches themes at runtime.
 *
 * Themes live on disk in the user-configurable directory returned by
 * Settings::getThemePath() (default: the app config dir + "themes/"). It ships
 * empty; users acquire themes separately and may point the setting anywhere.
 * Each theme lives in <themePath>/<id>/ and provides:
 *   metadata.xml     — standard LEDSpicer file (type "Theme"); name, format and
 *                      description live as root attributes (see ThemeMetadata)
 *   preview.png      — mandatory selector thumbnail
 * plus its CSS in one of two layouts, auto-detected at scan time:
 *   dual   — dark/theme.css and light/theme.css; the variant is picked per style.
 *   single — a lone theme.css beside metadata.xml, applied to every style.
 * Any extra assets (images, data) are the theme author's concern, not ours.
 *
 * CSS is loaded in two layers:
 *   1. style-base.css    from the GResource bundle at APPLICATION   priority
 *                        (structural bones, no colors/images)
 *   2. the theme's css   from disk                  at APPLICATION+1 priority
 *                        (colors, backgrounds, images)
 *
 * An empty theme id is the valid no-theme state: only the bundled base CSS loads.
 */
class ThemeManager {

public:

	/// Intrinsic theme description parsed from metadata.xml.

	struct ThemeMetadata {
		string
			id,
			name,
			description;   /// Shown as tooltip on the theme selector tile.
		bool dual {false}; /// true: ships dark/ + light/ variants. false: a single root theme.css.
	};

	ThemeManager(const ThemeManager&)            = delete;
	ThemeManager& operator=(const ThemeManager&) = delete;

	static ThemeManager& getInstance() noexcept { return instance; }

	/**
	 * Scans the themes directory and connects system dark-mode change tracking.
	 * Called once from main() before the main window is created.
	 */
	void initialize() noexcept;

	/**
	 * Re-scans the themes directory, refreshing the available theme list.
	 * Call after the themes directory changes or on an explicit refresh.
	 */
	void rescan() noexcept;

	/**
	 * Loads and applies the bundled base CSS plus the theme variant CSS to the default screen.
	 * Replaces any previously loaded theme providers.
	 * @param themeId  Directory name under the themes directory; empty selects the no-theme state.
	 * @param style    ThemeStyle::Auto detects system dark-mode preference.
	 * @return true on success (including the no-theme state), false if a theme's CSS could not be loaded.
	 */
	bool apply(const string& themeId, Config::Settings::ThemeStyle style) noexcept;

	/**
	 * @return All discovered themes, in discovery order.
	 */
	const vector<ThemeMetadata>& getThemes() const noexcept { return themes; }

	/**
	 * @param id Theme directory name to look up.
	 * @return true if a theme with this id was discovered by the last scan.
	 */
	bool hasTheme(const string& id) const noexcept;

	/**
	 * @return The id of the currently applied theme.
	 */
	const string& getCurrentThemeId() const noexcept { return currentThemeId; }

	/**
	 * @return The currently applied variant ("dark" or "light"), empty for a single-css theme.
	 */
	const string& getCurrentVariant() const noexcept { return currentVariant; }

private:

	ThemeManager() = default;

	static ThemeManager instance;

	/**
	 * Clears and repopulates the theme list from Settings::getThemePath().
	 */
	void scan() noexcept;

	/**
	 * @param id Theme directory name to look up.
	 * @return The discovered theme, or nullptr if no theme has this id.
	 */
	const ThemeMetadata* findTheme(const string& id) const noexcept;

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
	 * Parses themes/<id>/metadata.xml and appends to themes on success.
	 *
	 * @param themeDir Absolute path to the theme directory (e.g. <themePath>/darkblue/).
	 * @param themeId
	 * @param dual     Whether the theme ships dark/ + light/ variants (vs a single root theme.css).
	 */
	void parseMetadata(const string& themeDir, const string& themeId, bool dual) noexcept;

	/**
	 * Loads a CSS file from disk into a new provider on the default screen at the given priority.
	 *
	 * @param path Absolute path to the CSS file to load.
	 * @param priority Priority to apply the provider at (e.g. GTK_STYLE_PROVIDER_PRIORITY_APPLICATION).
	 * @return The loaded provider, or nullptr if loading failed.
	 */
	static Glib::RefPtr<Gtk::CssProvider> loadCss(const string& path, guint priority) noexcept;

	/**
	 * Loads a CSS file from the embedded GResource bundle into a new provider at the given priority.
	 *
	 * @param path Resource path of the CSS file.
	 * @param priority Priority to apply the provider at.
	 * @return The loaded provider, or nullptr if loading failed.
	 */
	static Glib::RefPtr<Gtk::CssProvider> loadCssResource(const string& path, guint priority) noexcept;

	/**
	 * Removes a provider from the default screen if non-null.
	 * @param provider
	 */
	static void removeProvider(Glib::RefPtr<Gtk::CssProvider>& provider) noexcept;
};

} // namespace
