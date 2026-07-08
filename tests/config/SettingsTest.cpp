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

#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "config/Settings.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;
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
	EXPECT_EQ(0, s.getLayoutGrid());
	EXPECT_EQ(1500u, s.getLayoutTestTimeout());
	EXPECT_FALSE(s.hasBinary());
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
		{"layoutGrid",         "30"},
		{"layoutTestTimeout",  "2.5"},
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
	EXPECT_EQ(30, s.getLayoutGrid());
	EXPECT_EQ(2500u, s.getLayoutTestTimeout());
	EXPECT_TRUE(s.hasBinary());

	// Round-trip: snapshot → reset → load → same values.
	const Values snap(s.begin(), s.end());
	s.load(Values{});
	s.load(snap);
	EXPECT_EQ("/usr/bin/ledspicerd", s.getBinaryPath());
	EXPECT_EQ(ThemeStyle::Dark,      s.getThemeStyle());
	EXPECT_EQ(30,                    s.getLayoutGrid());
	EXPECT_EQ(2500u,                 s.getLayoutTestTimeout());
}

TEST_F(SettingsTest, LoadFillsMissingKeysWithDefaults) {
	auto& s = Settings::get();
	s.load(Values{{"binaryPath", "/usr/bin/ledspicerd"}, {"themeStyle", "Light"}});

	EXPECT_EQ("/usr/bin/ledspicerd", s.getBinaryPath());
	EXPECT_TRUE(s.getDataDir().empty());   // missing → default ""
	EXPECT_EQ(ThemeStyle::Light, s.getThemeStyle());
	EXPECT_TRUE(s.shouldSaveBackup());     // missing → default True
	EXPECT_FALSE(s.shouldDebugFiles());    // missing → default False
	// interactive mode defaults to True → with binary set, testing is enabled
	EXPECT_TRUE(s.isInteractive());
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

TEST_F(SettingsTest, LayoutGridChangedCallback) {
	auto& s = Settings::get();

	int calls = 0;
	s.onLayoutGridChanged([&calls]() { ++calls; });

	s.setLayoutGrid(20);
	EXPECT_EQ(20, s.getLayoutGrid());
	EXPECT_EQ(1,  calls);

	s.setLayoutGrid(0);
	EXPECT_EQ(0, s.getLayoutGrid());
	EXPECT_EQ(2, calls);

	// Replace callback with null-equivalent (no crash on next set).
	s.onLayoutGridChanged(nullptr);
	s.setLayoutGrid(50);
	EXPECT_EQ(2, calls);
}

TEST_F(SettingsTest, LayoutTestTimeoutConversion) {
	auto& s = Settings::get();

	// Default: 1.5 s → 1500 ms.
	EXPECT_EQ(1500u, s.getLayoutTestTimeout());

	// Setter takes seconds, getter returns milliseconds.
	s.setLayoutTestTimeout(2.0);
	EXPECT_EQ(2000u, s.getLayoutTestTimeout());

	s.setLayoutTestTimeout(0.5);
	EXPECT_EQ(500u, s.getLayoutTestTimeout());

	// Single-decimal values survive a serialization round-trip.
	const Values snap(s.begin(), s.end());
	s.load(Values{});
	s.load(snap);
	EXPECT_EQ(500u, s.getLayoutTestTimeout());
}

TEST_F(SettingsTest, Writability) {
	namespace fs = std::filesystem;
	auto& s = Settings::get();

	// Volatile state survives load(); reset it so this test is order-independent.
	s.setConfigPath("");
	s.setCurrentProject("");

	// Nothing configured → nothing writable.
	EXPECT_FALSE(s.isProjectDirWritable());
	EXPECT_FALSE(s.isRootConfigWritable());

	const string base {::testing::TempDir() + "settingsWritability/"};
	fs::create_directories(base + "projects/arcade");

	// No system config anywhere → the config lives inside the project.
	s.setProjectsDir(base + "projects/");
	s.setCurrentProject("arcade");
	EXPECT_TRUE(s.isProjectDirWritable());
	EXPECT_EQ(Settings::ConfigSource::Project, s.getConfigSource());
	EXPECT_TRUE(s.isRootConfigWritable());

	// Missing project dir → probes the closest existing ancestor.
	s.setCurrentProject("brandNew");
	EXPECT_TRUE(s.isProjectDirWritable());

	// Creatable system config → System source; missing file probes its directory.
	s.setConfigPath(base + "ledspicer.conf");
	s.setCurrentProject("brandNew");
	EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());
	EXPECT_TRUE(s.isRootConfigWritable());

	// Permission bits are bypassed by root, so the negative case only runs unprivileged.
	if (geteuid() != 0) {
		fs::create_directories(base + "locked");
		std::ofstream(base + "locked/" CONFIG_FILE) << "";
		fs::permissions(base + "locked/" CONFIG_FILE, fs::perms::owner_read);
		s.setConfigPath(base + "locked/" CONFIG_FILE);
		s.setCurrentProject("brandNew");
		// An existing read-only system config stays the source, locked.
		EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());
		EXPECT_FALSE(s.isRootConfigWritable());
		fs::permissions(base + "locked/" CONFIG_FILE, fs::perms::owner_all);
	}

	fs::remove_all(base);
}

TEST_F(SettingsTest, ConfigSourceResolution) {
	namespace fs = std::filesystem;
	auto& s = Settings::get();

	const string base {::testing::TempDir() + "settingsConfigSource/"};
	fs::create_directories(base + "projects/existing");
	s.setProjectsDir(base + "projects/");

	// No project selected → System.
	s.setConfigPath("");
	s.setCurrentProject("");
	EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());

	// No binary (empty system path) → the config lives inside the project.
	s.setCurrentProject("existing");
	EXPECT_EQ(Settings::ConfigSource::Project, s.getConfigSource());

	// No config anywhere, system location writable → System.
	s.setConfigPath(base + CONFIG_FILE);
	s.setCurrentProject("existing");
	EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());

	// Same resolution when the project directory does not exist yet.
	s.setCurrentProject("brandNew");
	EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());

	// An existing system config wins over creation logic, for both
	// existing and not-yet-created projects.
	std::ofstream(base + CONFIG_FILE) << "";
	s.setCurrentProject("existing");
	EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());
	s.setCurrentProject("brandNew");
	EXPECT_EQ(Settings::ConfigSource::System, s.getConfigSource());

	// An embedded config wins even when the system config exists.
	std::ofstream(base + "projects/existing/" CONFIG_FILE) << "";
	s.setCurrentProject("existing");
	EXPECT_EQ(Settings::ConfigSource::Project, s.getConfigSource());

	// No config anywhere and a read-only system location → Project.
	// Permission bits are bypassed by root, so this only runs unprivileged.
	if (geteuid() != 0) {
		fs::create_directories(base + "locked");
		fs::permissions(base + "locked", fs::perms::owner_read | fs::perms::owner_exec);
		s.setConfigPath(base + "locked/" CONFIG_FILE);
		s.setCurrentProject("brandNew");
		EXPECT_EQ(Settings::ConfigSource::Project, s.getConfigSource());
		fs::permissions(base + "locked", fs::perms::owner_all);
	}

	fs::remove_all(base);
}

TEST_F(SettingsTest, BinaryAndInteractive) {
	auto& s = Settings::get();

	// Preference without a binary never enables testing.
	s.setBinaryPath("");
	s.setInteractiveMode(true);
	EXPECT_FALSE(s.hasBinary());
	EXPECT_FALSE(s.isInteractive());

	s.setBinaryPath("/usr/bin/ledspicerd");
	EXPECT_TRUE(s.hasBinary());
	EXPECT_TRUE(s.isInteractive());

	// Binary without the preference: testable machine, testing declined.
	s.setInteractiveMode(false);
	EXPECT_FALSE(s.isInteractive());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
