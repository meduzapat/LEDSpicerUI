/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ThemeManager.cpp
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

#include "ThemeManager.hpp"

using namespace LEDSpicerUI::Ui;

ThemeManager ThemeManager::instance;

void ThemeManager::initialize() noexcept {
	scan();

	auto gs = Gtk::Settings::get_default();
	auto onSystemChange = [this]() {
		if (Config::Settings::get().getThemeStyle() == Config::Settings::ThemeStyle::Auto)
			apply(Config::Settings::get().getThemeName(), Config::Settings::ThemeStyle::Auto);
	};
	gs->property_gtk_application_prefer_dark_theme().signal_changed().connect(onSystemChange);
	gs->property_gtk_theme_name().signal_changed().connect(onSystemChange);
}

void ThemeManager::scan() noexcept {
	themes.clear();
	const string themesDir{Config::Settings::get().getThemePath()};
	try {
		Glib::Dir d(themesDir);
		for (const auto& entry : d) {
			const string themeDir{themesDir + entry + "/"};
			if (not Glib::file_test(themeDir + "metadata.xml", Glib::FILE_TEST_IS_REGULAR))
				continue;
			if (not Glib::file_test(themeDir + "preview.png", Glib::FILE_TEST_IS_REGULAR)) {
				std::cerr <<
					"ThemeManager: skipping theme '"    << entry <<
					"' — missing mandatory preview.png" << std::endl;
				continue;
			}
			const bool dual {
				Glib::file_test(themeDir + "dark/theme.css",  Glib::FILE_TEST_IS_REGULAR) and
				Glib::file_test(themeDir + "light/theme.css", Glib::FILE_TEST_IS_REGULAR)
			};
			if (not dual and not Glib::file_test(themeDir + "theme.css", Glib::FILE_TEST_IS_REGULAR)) {
				std::cerr <<
					"ThemeManager: skipping theme '" << entry <<
					"' — needs dark/ + light/ theme.css or a single root theme.css" << std::endl;
				continue;
			}
			parseMetadata(themeDir, entry, dual);
		}
	}
	catch (const Glib::Error&) {}
}

bool ThemeManager::apply(const string& themeId, Config::Settings::ThemeStyle style) noexcept {

	removeProvider(baseProvider);
	removeProvider(themeProvider);

	baseProvider = loadCssResource(RESOURCE_PREFIX + "style-base.css", GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// No theme selected: the bundled base CSS alone renders a clean, un-themed UI.
	if (themeId.empty()) {
		currentThemeId.clear();
		currentVariant.clear();
		return true;
	}

	const ThemeMetadata* meta {findTheme(themeId)};
	const bool dual {meta and meta->dual};
	const string
		variant {dual ? resolveVariant(style) : ""},
		base    {Config::Settings::get().getThemePath() + themeId + "/"},
		cssPath {dual ? base + variant + "/theme.css" : base + "theme.css"};

	themeProvider = loadCss(cssPath, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
	if (not themeProvider) {
		currentThemeId.clear();
		currentVariant.clear();
		return false;
	}

	if (style == Config::Settings::ThemeStyle::Dark)
		Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme() = true;
	else if (style == Config::Settings::ThemeStyle::Light)
		Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme() = false;

	currentThemeId = themeId;
	currentVariant = variant;
	return true;
}

void ThemeManager::rescan() noexcept {
	scan();
}

bool ThemeManager::hasTheme(const string& id) const noexcept {
	return findTheme(id) != nullptr;
}

const ThemeManager::ThemeMetadata* ThemeManager::findTheme(const string& id) const noexcept {
	for (const auto& meta : themes)
		if (meta.id == id)
			return &meta;
	return nullptr;
}

string ThemeManager::resolveVariant(Config::Settings::ThemeStyle style) noexcept {

	if (style == Config::Settings::ThemeStyle::Dark)  return "dark";
	if (style == Config::Settings::ThemeStyle::Light) return "light";

	try {
		auto gsettings {Gio::Settings::create("org.gnome.desktop.interface")};
		if (gsettings->get_string("color-scheme") == "prefer-dark") return "dark";
	}
	catch (...) {}

	if (Gtk::Settings::get_default()->property_gtk_theme_name().get_value().lowercase().find("dark") != Glib::ustring::npos)
		return "dark";

	return "light";
}

void ThemeManager::parseMetadata(const string& themeDir, const string& themeId, bool dual) noexcept {
	try {
		XMLHelper doc(themeDir + "metadata.xml", THEME_TYPE);
		const auto& info {doc.getRootInfo()};
		if (info.getValue("format") != THEME_FORMAT) {
			throw Message(
				"Theme \"" + themeId + "\" has unsupported format \"" +
				info.getValue("format") + "\" (expected \"" THEME_FORMAT "\")"
			);
		}
		ThemeMetadata meta;
		meta.id          = themeId;
		meta.name        = info.getValue("name", themeId);
		meta.description = info.getValue("description");
		meta.dual        = dual;
		themes.push_back(std::move(meta));
	}
	// Malformed, foreign, or wrong-version metadata: skip the theme.
	catch (const Message&) {}
}

Glib::RefPtr<Gtk::CssProvider> ThemeManager::loadCss(const string& path, guint priority) noexcept {

	auto provider {Gtk::CssProvider::create()};
	try {
		provider->load_from_path(path);
	}
	catch (const Glib::Error& e) {
		std::cerr << "ThemeManager: failed to load " << path << ": " << e.what() << std::endl;
		return {};
	}
	Gtk::StyleContext::add_provider_for_screen(
		Gdk::Screen::get_default(),
		provider,
		priority
	);
	return provider;
}

Glib::RefPtr<Gtk::CssProvider> ThemeManager::loadCssResource(const string& path, guint priority) noexcept {

	auto provider {Gtk::CssProvider::create()};
	try {
		provider->load_from_resource(path);
	}
	catch (const Glib::Error& e) {
		std::cerr << "ThemeManager: failed to load resource " << path << ": " << e.what() << std::endl;
		return {};
	}
	Gtk::StyleContext::add_provider_for_screen(
		Gdk::Screen::get_default(),
		provider,
		priority
	);
	return provider;
}

void ThemeManager::removeProvider(Glib::RefPtr<Gtk::CssProvider>& provider) noexcept {
	if (not provider) return;
	Gtk::StyleContext::remove_provider_for_screen(Gdk::Screen::get_default(), provider);
	provider.reset();
}
