/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SettingsFileTest.cpp
 * @since     Feb 9, 2026
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
#include <cstdio>
#include "config/SettingsFile.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;

class SettingsFileTest : public ::testing::Test {

protected:

	string testPath;

	void SetUp() override {
		testPath = string(PACKAGE_SAMPLES_DIR) + "test_settings.conf";
		SettingsFile::setSettingsPath(testPath);
		if (Glib::file_test(testPath, Glib::FILE_TEST_EXISTS))
			std::remove(testPath.c_str());
		Settings::get().load(Values{});
	}

	void TearDown() override {
		if (Glib::file_test(testPath, Glib::FILE_TEST_EXISTS))
			std::remove(testPath.c_str());
		SettingsFile::setSettingsPath("");
	}
};

TEST_F(SettingsFileTest, MissingFile) {
	EXPECT_FALSE(SettingsFile::initialize());
	const auto& s = Settings::get();
	EXPECT_TRUE(s.getBinaryPath().empty());
	EXPECT_FALSE(s.hasBinary());
	EXPECT_TRUE(s.shouldSaveBackup());
	EXPECT_FALSE(s.shouldDebugFiles());
}

TEST_F(SettingsFileTest, LoadFromFile) {
	Glib::file_set_contents(testPath,
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<LEDSpicer version=\"1.1\" type=\"Settings\""
		" binaryPath=\"/usr/bin/ledspicerd\""
		" dataDir=\"/usr/share/ledspicer/\"/>"
	);
	EXPECT_TRUE(SettingsFile::initialize());
	const auto& s = Settings::get();
	EXPECT_EQ("/usr/bin/ledspicerd",   s.getBinaryPath());
	EXPECT_EQ("/usr/share/ledspicer/", s.getDataDir());
}

TEST_F(SettingsFileTest, SaveToFile) {
	Settings::get().setBinaryPath("/usr/bin/ledspicerd");
	SettingsFile::save();
	EXPECT_TRUE(Glib::file_test(testPath, Glib::FILE_TEST_EXISTS));
	const string content = Glib::file_get_contents(testPath);
	EXPECT_NE(string::npos, content.find("binaryPath=\"/usr/bin/ledspicerd\""));
	EXPECT_NE(string::npos, content.find("type=\"" UI_CONFIG_TYPE "\""));
}

TEST_F(SettingsFileTest, SaveRoundTrip) {
	auto& s = Settings::get();
	s.setBinaryPath("/usr/bin/ledspicerd");
	s.setDataDir("/usr/share/ledspicer/");
	s.setDebugFiles(true);
	s.setThemeStyle(Settings::ThemeStyle::Dark);
	s.setCurrentProject("arcade");
	SettingsFile::save();

	s.load(Values{});
	EXPECT_TRUE(s.getBinaryPath().empty());
	EXPECT_TRUE(s.getDefaultProject().empty());

	EXPECT_TRUE(SettingsFile::initialize());
	EXPECT_EQ("/usr/bin/ledspicerd",   s.getBinaryPath());
	EXPECT_EQ("/usr/share/ledspicer/", s.getDataDir());
	EXPECT_TRUE(s.shouldDebugFiles());
	EXPECT_EQ(Settings::ThemeStyle::Dark, s.getThemeStyle());
	EXPECT_EQ("arcade",                s.getDefaultProject());
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
