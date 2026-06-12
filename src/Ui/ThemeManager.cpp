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

#include <gio/gio.h>
#include "ThemeManager.hpp"

using namespace LEDSpicerUI::Ui;

ThemeManager ThemeManager::instance;

void ThemeManager::initialize() noexcept {
	themes.clear();
	constexpr const char* themesPrefix{"/org/ledspicer/ui/themes/"};
	GError* err{nullptr};
	gchar** children{g_resources_enumerate_children(themesPrefix, G_RESOURCE_LOOKUP_FLAGS_NONE, &err)};
	if (children) {
		for (int i = 0; children[i]; ++i) {
			string themeId{children[i]};
			if (not themeId.empty() and themeId.back() == '/')
				themeId.pop_back();
			parseMetadata(string{themesPrefix} + themeId + "/", themeId);
		}
		g_strfreev(children);
	}
	else {
		if (err) g_error_free(err);
	}

	auto gs = Gtk::Settings::get_default();
	auto onSystemChange = [this]() {
		if (Config::Settings::get().getThemeStyle() == Config::Settings::ThemeStyle::Auto)
			apply(Config::Settings::get().getThemeName(), Config::Settings::ThemeStyle::Auto);
	};
	gs->property_gtk_application_prefer_dark_theme().signal_changed().connect(onSystemChange);
	gs->property_gtk_theme_name().signal_changed().connect(onSystemChange);
}

bool ThemeManager::apply(const string& themeId, Config::Settings::ThemeStyle style) noexcept {

	const string& id {themeId.empty() ? DEFAULT : themeId};

	removeProvider(baseProvider);
	removeProvider(themeProvider);

	baseProvider = loadCss("/org/ledspicer/ui/style-base.css", GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	if (not baseProvider)
		baseProvider = loadCss("/org/ledspicer/ui/style.css", GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	const string variant{resolveVariant(style)};
	const string cssPath{"/org/ledspicer/ui/themes/" + id + "/" + variant + "/theme.css"};

	themeProvider = loadCss(cssPath, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
	if (not themeProvider) {
		if (id != DEFAULT)
			return apply(DEFAULT, style);
		currentThemeId.clear();
		currentVariant.clear();
		return false;
	}

	if (style == Config::Settings::ThemeStyle::Dark)
		Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme() = true;
	else if (style == Config::Settings::ThemeStyle::Light)
		Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme() = false;

	currentThemeId = id;
	currentVariant = variant;
	return true;
}

string ThemeManager::resolveVariant(Config::Settings::ThemeStyle style) noexcept {

	if (style == Config::Settings::ThemeStyle::Dark) return "dark";
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

void ThemeManager::parseMetadata(const string& resourcePrefix, const string& themeId) noexcept {
	GError* err{nullptr};
	GBytes* bytes{g_resources_lookup_data(
		(resourcePrefix + "metadata.xml").c_str(),
		G_RESOURCE_LOOKUP_FLAGS_NONE,
		&err
	)};
	if (not bytes) {
		if (err) g_error_free(err);
		return;
	}
	gsize size{0};
	const gchar* data{static_cast<const gchar*>(g_bytes_get_data(bytes, &size))};
	tinyxml2::XMLDocument doc;
	const bool parseOk{doc.Parse(data, size) == tinyxml2::XML_SUCCESS};
	g_bytes_unref(bytes);
	if (not parseOk)
		return;

	const auto root = doc.FirstChildElement("theme");
	if (not root)
		return;

	ThemeMetadata meta;
	meta.id   = themeId;
	meta.name = root->Attribute("name") ? root->Attribute("name") : themeId;
	if (const auto desc = root->FirstChildElement("description"); desc and desc->GetText())
		meta.description = desc->GetText();

	themes.push_back(std::move(meta));
}

Glib::RefPtr<Gtk::CssProvider> ThemeManager::loadCss(const string& resourcePath, guint priority) noexcept {

	auto provider {Gtk::CssProvider::create()};
	try {
		provider->load_from_resource(resourcePath);
	}
	catch (const Glib::Error& e) {
		std::cerr << "ThemeManager: failed to load " << resourcePath << ": " << e.what() << std::endl;
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
