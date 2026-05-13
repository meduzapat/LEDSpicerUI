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

using namespace LEDSpicerUI::Config;
using Mode       = Settings::Mode;
using ThemeStyle = Settings::ThemeStyle;

// Mock data representing a fully populated saved settings map.
static const StringUMap MOCK_FULL_MAP = {
	{"binaryPath",        "/usr/bin/ledspicerd"},
	{"dataDir",           "/usr/share/ledspicer/"},
	{"projectsDir",       "/home/user/projects/"},
	{"interactiveMode",   HUMAN_FALSE},
	{"themeStyle",        "Dark"},
	{"cleanProjectDir",   HUMAN_TRUE},
	{"preserveEmptyDir",  HUMAN_TRUE},
	{"removeInvalidItems",HUMAN_TRUE},
	{"saveBackup",        HUMAN_FALSE},
	{"debugFiles",        HUMAN_TRUE},
};

// Mock data with only a subset of keys — simulates a partial or legacy saved file.
static const StringUMap MOCK_PARTIAL_MAP = {
	{"binaryPath",      "/usr/bin/ledspicerd"},
	{"themeStyle",      "Light"},
	{"cleanProjectDir", HUMAN_TRUE},
	{"saveBackup",      HUMAN_FALSE},
};

class SettingsTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Reset to defaults before each test (empty map → all defaults).
		Settings::get().fromMap({});
	}
};

TEST_F(SettingsTest, DefaultPaths) {
	EXPECT_TRUE(Settings::get().getBinaryPath().empty());
	EXPECT_TRUE(Settings::get().getDataDir().empty());
	EXPECT_TRUE(Settings::get().getProjectsDir().empty());
}

TEST_F(SettingsTest, DefaultMode) {
	EXPECT_EQ(Mode::Portable, Settings::get().getMode());
	EXPECT_TRUE(Settings::get().isPortable());
}

TEST_F(SettingsTest, DefaultInteractiveMode) {
	EXPECT_TRUE(Settings::get().isInteractiveMode());
}

TEST_F(SettingsTest, DefaultThemeStyle) {
	EXPECT_EQ(ThemeStyle::Auto, Settings::get().getThemeStyle());
}

TEST_F(SettingsTest, DefaultBooleans) {
	EXPECT_FALSE(Settings::get().shouldCleanProjectDir());
	EXPECT_FALSE(Settings::get().shouldPreserveEmptyDir());
	EXPECT_FALSE(Settings::get().shouldRemoveInvalidItems());
	EXPECT_TRUE (Settings::get().shouldSaveBackup());
	EXPECT_FALSE(Settings::get().shouldDebugFiles());
}

TEST_F(SettingsTest, SetGetPaths) {
	Settings::get().setBinaryPath("/usr/bin/ledspicerd");
	Settings::get().setDataDir("/usr/share/ledspicer/");
	Settings::get().setProjectsDir("/home/user/projects/");

	EXPECT_EQ("/usr/bin/ledspicerd",   Settings::get().getBinaryPath());
	EXPECT_EQ("/usr/share/ledspicer/", Settings::get().getDataDir());
	EXPECT_EQ("/home/user/projects/",  Settings::get().getProjectsDir());
}

TEST_F(SettingsTest, SetGetThemeStyle) {
	Settings::get().setThemeStyle(ThemeStyle::Dark);
	EXPECT_EQ(ThemeStyle::Dark, Settings::get().getThemeStyle());

	Settings::get().setThemeStyle(ThemeStyle::Light);
	EXPECT_EQ(ThemeStyle::Light, Settings::get().getThemeStyle());

	Settings::get().setThemeStyle(ThemeStyle::Auto);
	EXPECT_EQ(ThemeStyle::Auto, Settings::get().getThemeStyle());
}

TEST_F(SettingsTest, SetGetBooleans) {
	Settings::get().setCleanProjectDir(true);
	Settings::get().setPreserveEmptyDir(true);
	Settings::get().setRemoveInvalidItems(true);
	Settings::get().setSaveBackup(false);
	Settings::get().setDebugFiles(true);

	EXPECT_TRUE (Settings::get().shouldCleanProjectDir());
	EXPECT_TRUE (Settings::get().shouldPreserveEmptyDir());
	EXPECT_TRUE (Settings::get().shouldRemoveInvalidItems());
	EXPECT_FALSE(Settings::get().shouldSaveBackup());
	EXPECT_TRUE (Settings::get().shouldDebugFiles());
}

TEST_F(SettingsTest, ModePortableWhenNoBinary) {
	Settings::get().setBinaryPath("");
	EXPECT_EQ(Mode::Portable, Settings::get().getMode());
	EXPECT_TRUE(Settings::get().isPortable());
}

TEST_F(SettingsTest, ModeIterativeWhenBinaryAndInteractive) {
	Settings::get().setInteractiveMode(true);
	Settings::get().setBinaryPath("/usr/bin/ledspicerd");
	EXPECT_EQ(Mode::Iterative, Settings::get().getMode());
}

TEST_F(SettingsTest, ModeLocalWhenBinaryNoInteractive) {
	Settings::get().setInteractiveMode(false);
	Settings::get().setBinaryPath("/usr/bin/ledspicerd");
	EXPECT_EQ(Mode::Local, Settings::get().getMode());
	EXPECT_FALSE(Settings::get().isPortable());
}

TEST_F(SettingsTest, ModeUpdatesWhenInteractiveModeChanges) {
	Settings::get().setBinaryPath("/usr/bin/ledspicerd");
	EXPECT_EQ(Mode::Iterative, Settings::get().getMode());

	Settings::get().setInteractiveMode(false);
	EXPECT_EQ(Mode::Local, Settings::get().getMode());

	Settings::get().setInteractiveMode(true);
	EXPECT_EQ(Mode::Iterative, Settings::get().getMode());
}

TEST_F(SettingsTest, ModeReturnsPortableWhenBinaryCleared) {
	Settings::get().setBinaryPath("/usr/bin/ledspicerd");
	EXPECT_EQ(Mode::Iterative, Settings::get().getMode());

	Settings::get().setBinaryPath("");
	EXPECT_TRUE(Settings::get().isPortable());
}

TEST_F(SettingsTest, FromMapLoadsAllValues) {
	Settings::get().fromMap(MOCK_FULL_MAP);

	EXPECT_EQ("/usr/bin/ledspicerd",   Settings::get().getBinaryPath());
	EXPECT_EQ("/usr/share/ledspicer/", Settings::get().getDataDir());
	EXPECT_EQ("/home/user/projects/",  Settings::get().getProjectsDir());
	EXPECT_FALSE(Settings::get().isInteractiveMode());
	EXPECT_EQ(ThemeStyle::Dark,        Settings::get().getThemeStyle());
	EXPECT_TRUE (Settings::get().shouldCleanProjectDir());
	EXPECT_TRUE (Settings::get().shouldPreserveEmptyDir());
	EXPECT_TRUE (Settings::get().shouldRemoveInvalidItems());
	EXPECT_FALSE(Settings::get().shouldSaveBackup());
	EXPECT_TRUE (Settings::get().shouldDebugFiles());
}

TEST_F(SettingsTest, FromMapEmptyUsesDefaults) {

	Settings::get().fromMap({});
	EXPECT_TRUE(Settings::get().getBinaryPath().empty());
	EXPECT_TRUE(Settings::get().isInteractiveMode());
	EXPECT_FALSE(Settings::get().shouldCleanProjectDir());
	EXPECT_TRUE (Settings::get().shouldSaveBackup());
	EXPECT_EQ(Mode::Portable,   Settings::get().getMode());
	EXPECT_EQ(ThemeStyle::Auto, Settings::get().getThemeStyle());
}

TEST_F(SettingsTest, FromMapThemeStyleLight) {
	Settings::get().fromMap({{"themeStyle", "light"}});  // single-key mock
	EXPECT_EQ(ThemeStyle::Light, Settings::get().getThemeStyle());
}

TEST_F(SettingsTest, FromMapUnknownThemeStyleFallsBackToAuto) {
	Settings::get().fromMap({{"themeStyle", "solarized"}});  // single-key mock
	EXPECT_EQ(ThemeStyle::Auto, Settings::get().getThemeStyle());
}

TEST_F(SettingsTest, ToMapContainsAllKeys) {
	StringUMap m = Settings::get().toMap();

	EXPECT_TRUE(m.count("binaryPath"));
	EXPECT_TRUE(m.count("dataDir"));
	EXPECT_TRUE(m.count("projectsDir"));
	EXPECT_TRUE(m.count("interactiveMode"));
	EXPECT_TRUE(m.count("themeStyle"));
	EXPECT_TRUE(m.count("cleanProjectDir"));
	EXPECT_TRUE(m.count("preserveEmptyDir"));
	EXPECT_TRUE(m.count("removeInvalidItems"));
	EXPECT_TRUE(m.count("saveBackup"));
	EXPECT_TRUE(m.count("debugFiles"));
}

TEST_F(SettingsTest, RoundTrip) {
	Settings::get().fromMap(MOCK_PARTIAL_MAP);
	Settings::get().fromMap(Settings::get().toMap());

	EXPECT_EQ("/usr/bin/ledspicerd", Settings::get().getBinaryPath());
	EXPECT_EQ(ThemeStyle::Light,     Settings::get().getThemeStyle());
	EXPECT_TRUE (Settings::get().shouldCleanProjectDir());
	EXPECT_FALSE(Settings::get().shouldSaveBackup());
}
