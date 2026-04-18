/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFileTest.cpp
 * @since     Feb 24, 2025
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
#include "config/ConfigFile.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;

class ConfigFileTest : public ::testing::Test {

protected:

	void SetUp() override {
		configFile = std::make_unique<ConfigFile>(PACKAGE_SAMPLES_DIR "data/config.xml");
	}

	void TearDown() override {
		configFile.reset();
	}

	std::unique_ptr<ConfigFile> configFile;
};

TEST_F(ConfigFileTest, DefaultProfileIsExtracted) {
	EXPECT_EQ("default", configFile->getDefaultProfile());
}

TEST_F(ConfigFileTest, ProcessLookupRunEveryValue) {
	EXPECT_EQ("500", configFile->getProcessLookupRunEvery());
}

TEST_F(ConfigFileTest, SettingsAreExtracted) {
	auto settings = configFile->getSettings();

	EXPECT_EQ("true",        settings["craftProfile"]);
	EXPECT_EQ("true",        settings["colorsFile"]);
	EXPECT_EQ("1000",        settings["groupId"]);
	EXPECT_EQ("Info",        settings["logLevel"]);
	EXPECT_EQ("1000",        settings["userId"]);
	EXPECT_EQ("10",          settings["fps"]);
	EXPECT_EQ("1.1",         settings["version"]);
	EXPECT_EQ("basicColors", settings["colors"]);
	EXPECT_EQ("Configuration", settings["type"]);
	EXPECT_EQ("Red, Blue, Green, Yellow, White, Black", settings["randomColors"]);
	EXPECT_EQ("file,mame",   settings["dataSource"]);
	EXPECT_EQ("16161",       settings["port"]);
}

TEST_F(ConfigFileTest, DevicesAreProcessed) {
	auto& devices = configFile->getData(COLLECTION_DEVICES);

	EXPECT_EQ(3, devices.size());

	EXPECT_EQ("UltimarcPacDrive", devices[0][NAME]);
	EXPECT_EQ("1",                devices[0][ID]);
	EXPECT_EQ("121",              devices[0]["changePoint"]);

	EXPECT_EQ("UltimarcPacDrive", devices[1][NAME]);
	EXPECT_EQ("2",                devices[1][ID]);
	EXPECT_EQ("64",               devices[1]["changePoint"]);

	EXPECT_EQ("UltimarcUltimate", devices[2][NAME]);
	EXPECT_EQ("1",                devices[2][ID]);

	string deviceId    = Defaults::createHardwareUniqueId({{NAME, "UltimarcPacDrive"}, {ID, "1"}});
	string elementsKey = Defaults::createCommonUniqueId({deviceId, COLLECTION_ELEMENT});

	try {
		auto& elements = configFile->getData(elementsKey);
		EXPECT_FALSE(elements.empty());
		bool foundP1Button1 = false;
		for (const auto& element : elements) {
			if (element.at(NAME) == "P1_BUTTON1") {
				foundP1Button1 = true;
				EXPECT_EQ("1", element.at("type"));
				EXPECT_EQ("2", element.at("led"));
				break;
			}
		}
		EXPECT_TRUE(foundP1Button1) << "P1_BUTTON1 element not found";
	}
	catch (const std::exception& e) {
		FAIL() << "Failed to access elements with key: " << elementsKey << ", error: " << e.what();
	}
}

TEST_F(ConfigFileTest, GroupsAreProcessed) {
	EXPECT_EQ("default", configFile->getDefaultProfile());

	auto& groups = configFile->getData(COLLECTION_GROUP);
	EXPECT_EQ(4, groups.size());

	EXPECT_EQ("All",     groups[0][NAME]);
	EXPECT_EQ("CREDITS", groups[1][NAME]);
	EXPECT_EQ("STARTS",  groups[2][NAME]);
	EXPECT_EQ("PLAYER1", groups[3][NAME]);

	string creditsGroupKey = Defaults::createCommonUniqueId({"CREDITS", COLLECTION_GROUP_LINKS});

	try {
		auto& creditElements = configFile->getData(creditsGroupKey);
		EXPECT_EQ(3, creditElements.size());

		EXPECT_EQ("P1_CREDIT", creditElements[0][NAME]);
		EXPECT_EQ("P2_CREDIT", creditElements[1][NAME]);
		EXPECT_EQ("P3_CREDIT", creditElements[2][NAME]);
	}
	catch (const std::exception& e) {
		FAIL() << "Failed to access group elements with key: " << creditsGroupKey << ", error: " << e.what();
	}
}
