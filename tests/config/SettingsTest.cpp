/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SettingsTest.cpp
 * @since     May 10, 2026
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

#include <gtest/gtest.h>
#include "config/Settings.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;
using Mode       = Settings::Mode;
using ThemeStyle = Settings::ThemeStyle;

class SettingsTest : public ::testing::Test {

protected:

	void SetUp() override {
		Settings::get().load(Values{});
	}

};

TEST_F(SettingsTest, DefaultState) {
	const auto& s {Settings::get()};
	EXPECT_TRUE(s.getBinaryPath().empty());
	EXPECT_TRUE(s.getDataDir().empty());
	EXPECT_TRUE(s.getProjectsDir().empty());
	EXPECT_TRUE(s.getDefaultProject().empty());
	EXPECT_EQ(ThemeStyle::Auto, s.getThemeStyle());
	EXPECT_TRUE (s.isInteractiveMode());
	EXPECT_TRUE (s.shouldPreserveEmptyDir());
	EXPECT_TRUE (s.shouldRemoveInvalidItems());
	EXPECT_TRUE (s.shouldSaveBackup());
	EXPECT_FALSE(s.shouldDebugFiles());
	EXPECT_EQ(Mode::Portable, s.getMode());
	EXPECT_TRUE(s.isPortable());
}

TEST_F(SettingsTest, LoadAndSerialize) {
	auto& s = Settings::get();
	s.load(Values{
		{"binaryPath",         "/usr/bin/ledspicerd"},
		{"dataDir",            "/usr/share/ledspicer/"},
		{"projectsDir",        "/home/user/projects/"},
		{"defaultProject",     "mygame"},
		{"interactiveMode",    HUMAN_FALSE},
		{"themeStyle",         "Dark"},
		{"preserveEmptyDir",   HUMAN_FALSE},
		{"removeInvalidItems", HUMAN_FALSE},
		{"saveBackup",         HUMAN_FALSE},
		{"debugFiles",         HUMAN_TRUE},
	});

	EXPECT_EQ("/usr/bin/ledspicerd",   s.getBinaryPath());
	EXPECT_EQ("/usr/share/ledspicer/", s.getDataDir());
	EXPECT_EQ("/home/user/projects/",  s.getProjectsDir());
	EXPECT_EQ("mygame",                s.getDefaultProject());
	EXPECT_EQ(ThemeStyle::Dark,        s.getThemeStyle());
	EXPECT_FALSE(s.isInteractiveMode());
	EXPECT_FALSE(s.shouldPreserveEmptyDir());
	EXPECT_FALSE(s.shouldRemoveInvalidItems());
	EXPECT_FALSE(s.shouldSaveBackup());
	EXPECT_TRUE (s.shouldDebugFiles());
	EXPECT_EQ(Mode::Local, s.getMode());

	// Round-trip: snapshot → reset → load → same values.
	const Values snap(s.begin(), s.end());
	s.load(Values{});
	s.load(snap);
	EXPECT_EQ("/usr/bin/ledspicerd", s.getBinaryPath());
	EXPECT_EQ(ThemeStyle::Dark,      s.getThemeStyle());
	EXPECT_EQ(Mode::Local,           s.getMode());
}

TEST_F(SettingsTest, LoadFillsMissingKeysWithDefaults) {
	auto& s = Settings::get();
	s.load(Values{{"binaryPath", "/usr/bin/ledspicerd"}, {"themeStyle", "Light"}});

	EXPECT_EQ("/usr/bin/ledspicerd", s.getBinaryPath());
	EXPECT_TRUE(s.getDataDir().empty());   // missing → default ""
	EXPECT_EQ(ThemeStyle::Light, s.getThemeStyle());
	EXPECT_TRUE(s.shouldSaveBackup());     // missing → default True
	EXPECT_FALSE(s.shouldDebugFiles());    // missing → default False
	// interactive mode defaults to True → with binary set, mode is Iterative
	EXPECT_EQ(Mode::Iterative, s.getMode());
}

TEST_F(SettingsTest, ModeDerivation) {
	auto& s = Settings::get();

	// No binary → Portable regardless of interactive preference.
	s.setBinaryPath("");
	s.setInteractiveMode(true);
	EXPECT_EQ(Mode::Portable, s.getMode());
	EXPECT_TRUE(s.isPortable());

	// Binary + interactive → Iterative.
	s.setBinaryPath("/usr/bin/ledspicerd");
	EXPECT_EQ(Mode::Iterative, s.getMode());
	EXPECT_TRUE(s.isIterative());

	// Binary, interactive off → Local.
	s.setInteractiveMode(false);
	EXPECT_EQ(Mode::Local, s.getMode());

	// Clear binary → Portable again.
	s.setBinaryPath("");
	EXPECT_EQ(Mode::Portable, s.getMode());
}

TEST_F(SettingsTest, VolatileState) {
	auto& s = Settings::get();

	s.setConfigPath("/etc/ledspicer/ledspicer.conf");
	EXPECT_EQ("/etc/ledspicer/ledspicer.conf", s.getConfigPath());

	s.setProjectsDir("/home/user/projects/");
	s.setCurrentProject("arcade");
	EXPECT_EQ("arcade", s.getCurrentProject());
	EXPECT_EQ("arcade", s.getDefaultProject()); // setCurrentProject also writes DEFAULT_PROJECT
	EXPECT_EQ("/home/user/projects/arcade/", s.getProjectDir());

	// Portable mode → active config is inside the project dir.
	EXPECT_TRUE(s.isPortable());
	EXPECT_EQ("/home/user/projects/arcade/" CONFIG_FILE, s.getActiveConfigPath());

	// Local mode → active config is configPath.
	s.setBinaryPath("/usr/bin/ledspicerd");
	s.setInteractiveMode(false);
	EXPECT_TRUE(s.isLocal());
	EXPECT_EQ("/etc/ledspicer/ledspicer.conf", s.getActiveConfigPath());

	StringVector files = {"colors.ini", "extra.ini"};
	s.setColorFiles(files);
	EXPECT_EQ(2u, s.getColorFiles().size());
	EXPECT_EQ("colors.ini", s.getColorFiles()[0]);

	s.setDataDirStatus(true, true, false);
	EXPECT_TRUE (s.getHasGameData());
	EXPECT_TRUE (s.getHasColors());
	EXPECT_FALSE(s.getHasControls());
}

TEST_F(SettingsTest, ColorFilesChangedCallback) {
	auto& s = Settings::get();

	int calls = 0;
	s.onColorFilesChanged([&calls]() { ++calls; });

	// Empty list → callback not fired.
	s.setColorFiles({});
	EXPECT_EQ(0, calls);

	// Non-empty list → callback fired.
	s.setColorFiles({"a.ini"});
	EXPECT_EQ(1, calls);

	// Fires again on subsequent non-empty sets.
	s.setColorFiles({"b.ini", "c.ini"});
	EXPECT_EQ(2, calls);

	// Replace callback with null-equivalent (no crash on next set).
	s.onColorFilesChanged(nullptr);
	s.setColorFiles({"d.ini"});
	EXPECT_EQ(2, calls);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
