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
#include <fstream>
#include "config/SettingsFile.hpp"

using namespace LEDSpicerUI::Config;

/**
 * SettingsFileTest
 * Tests SettingsFile unique functionality (UI config file operations).
 * XML parsing is already tested in XMLHelperTest.
 */
class SettingsFileTest : public ::testing::Test {

protected:

	string testConfigFile;

	void SetUp() override {
		testConfigFile = PACKAGE_SAMPLES_DIR "test_settings.conf";
	}

	void TearDown() override {
		if (Glib::file_test(testConfigFile, Glib::FILE_TEST_EXISTS))
			std::remove(testConfigFile.c_str());
	}
};

TEST_F(SettingsFileTest, SaveCreatesValidFile) {
	StringUMap settings = {
		{"binaryPath",  "/usr/bin/ledspicerd"},
		{"dataDir",     "/usr/share/ledspicer"},
		{"projectsDir", "/home/user/projects"}
	};

	EXPECT_NO_THROW(SettingsFile::save(testConfigFile, settings));
	EXPECT_TRUE(Glib::file_test(testConfigFile, Glib::FILE_TEST_EXISTS));
}

TEST_F(SettingsFileTest, SaveWritesCorrectContent) {
	StringUMap settings = {
		{"binaryPath", "/usr/bin/ledspicerd"},
		{"dataDir",    "/usr/share/ledspicer"}
	};

	SettingsFile::save(testConfigFile, settings);

	string content = Glib::file_get_contents(testConfigFile);

	EXPECT_NE(string::npos, content.find("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	EXPECT_NE(string::npos, content.find("<" PACKAGE_NAME));
	EXPECT_NE(string::npos, content.find("binaryPath=\"/usr/bin/ledspicerd\""));
	EXPECT_NE(string::npos, content.find("dataDir=\"/usr/share/ledspicer\""));
	EXPECT_NE(string::npos, content.find("/>"));
}

TEST_F(SettingsFileTest, SaveSkipsEmptyValues) {
	StringUMap settings = {
		{"binaryPath",  "/usr/bin/ledspicerd"},
		{"dataDir",     ""},
		{"projectsDir", "/home/user/projects"}
	};

	SettingsFile::save(testConfigFile, settings);

	string content = Glib::file_get_contents(testConfigFile);

	EXPECT_NE(string::npos, content.find("binaryPath=\"/usr/bin/ledspicerd\""));
	EXPECT_EQ(string::npos, content.find("dataDir=")) << "Empty value should be skipped";
	EXPECT_NE(string::npos, content.find("projectsDir=\"/home/user/projects\""));
}

TEST_F(SettingsFileTest, LoadSavedSettings) {
	StringUMap originalSettings = {
		{"binaryPath",  "/usr/bin/ledspicerd"},
		{"dataDir",     "/usr/share/ledspicer"},
		{"projectsDir", "/home/user/projects"}
	};

	SettingsFile::save(testConfigFile, originalSettings);

	EXPECT_NO_THROW({
		SettingsFile config(testConfigFile);
		StringUMap loadedSettings = config.getSettings();

		EXPECT_EQ("/usr/bin/ledspicerd",   loadedSettings["binaryPath"]);
		EXPECT_EQ("/usr/share/ledspicer",  loadedSettings["dataDir"]);
		EXPECT_EQ("/home/user/projects",   loadedSettings["projectsDir"]);
	});
}

TEST_F(SettingsFileTest, GetConfigFilePathReturnsValidPath) {
	string configPath = SettingsFile::getConfigFilePath();

	EXPECT_FALSE(configPath.empty());
	EXPECT_NE(string::npos, configPath.find(PACKAGE_NAME))
		<< "Config path should contain package name";
	EXPECT_NE(string::npos, configPath.find(UI_CONFIG_FILE))
		<< "Config path should contain config filename";
}

TEST_F(SettingsFileTest, ConfigExistsReturnsFalseWhenMissing) {
	string actualConfigPath = SettingsFile::getConfigFilePath();
	if (not Glib::file_test(actualConfigPath, Glib::FILE_TEST_EXISTS))
		EXPECT_FALSE(SettingsFile::configExists());
}

TEST_F(SettingsFileTest, SaveHandlesSpecialCharacters) {
	StringUMap settings = {
		{"path", "/home/user/My Projects/LEDSpicer Files"},
		{"note", "Test & Development"}
	};

	EXPECT_NO_THROW(SettingsFile::save(testConfigFile, settings));

	SettingsFile config(testConfigFile);
	StringUMap loadedSettings = config.getSettings();

	EXPECT_EQ("/home/user/My Projects/LEDSpicer Files", loadedSettings["path"]);
	EXPECT_EQ("Test & Development",                     loadedSettings["note"]);
}

TEST_F(SettingsFileTest, SaveEmptySettings) {
	StringUMap emptySettings;

	EXPECT_NO_THROW(SettingsFile::save(testConfigFile, emptySettings));

	string content = Glib::file_get_contents(testConfigFile);

	EXPECT_NE(string::npos, content.find("<?xml"));
	EXPECT_NE(string::npos, content.find("<" PACKAGE_NAME));
	EXPECT_NE(string::npos, content.find("/>"));
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
