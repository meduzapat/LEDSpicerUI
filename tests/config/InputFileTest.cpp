/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputFileTest.cpp
 * @since     Feb 25, 2025
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
#include "config/InputFile.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;

class InputFileTest : public ::testing::Test {

protected:

	void SetUp() override {
		inputMulti  = std::make_unique<InputFile>(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputMulti.xml",  nullptr);
		inputSingle = std::make_unique<InputFile>(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputSingle.xml", nullptr);
	}

	void TearDown() override {
		inputMulti.reset();
		inputSingle.reset();
	}

	std::unique_ptr<InputFile> inputMulti;
	std::unique_ptr<InputFile> inputSingle;

	/*
	 * Builds the base collection key for a file at root level (nullptr parent).
	 * Matches InputFile::InputFile() → createCommonUniqueId({"", filename}).
	 */
	static string baseId(const string& filename) {
		return Defaults::createCommonUniqueId({"", filename});
	}
};

TEST_F(InputFileTest, MultiSourceInputIsLoaded) {
	auto& inputData = inputMulti->getData(COLLECTION_INPUT);
	ASSERT_FALSE(inputData.empty());
	ASSERT_FALSE(inputData[0].empty());

	EXPECT_EQ("Credits", inputData[0][NAME]);
	EXPECT_EQ("Normal",  inputData[0]["speed"]);
	EXPECT_EQ("0,1|2,3", inputData[0]["linkedTriggers"]);
	EXPECT_EQ("Single",  inputData[0]["mode"]);
	EXPECT_EQ("True",    inputData[0]["once"]);
	EXPECT_EQ("False",   inputData[0]["alwaysOn"]);
	EXPECT_EQ("1",       inputData[0]["coinsPerCredit"]);

	EXPECT_EQ("inputMulti", inputMulti->getFilename());
}

TEST_F(InputFileTest, SourcesAreExtracted) {
	const string eventName(Defaults::createCommonUniqueId({baseId("inputMulti"), COLLECTION_INPUT_SOURCES}));

	EXPECT_EQ("hardware1", inputMulti->getData(eventName)[0]["source"]);
	EXPECT_EQ("hardware2", inputMulti->getData(eventName)[1]["source"]);
}

TEST_F(InputFileTest, FirstSourceMapsAreProcessed) {
	const string mapName(Defaults::createCommonUniqueId({baseId("inputMulti"), "0", COLLECTION_INPUT_MAPS}));

	auto& maps = inputMulti->getData(mapName);
	ASSERT_EQ(3, maps.size());

	EXPECT_EQ("Element", maps[0]["type"]);
	EXPECT_EQ("P1_COIN", maps[0]["target"]);
	EXPECT_EQ("309",     maps[0]["trigger"]);
	EXPECT_EQ("White",   maps[0]["color"]);
	EXPECT_EQ("Combine", maps[0]["filter"]);

	EXPECT_EQ("Element",  maps[1]["type"]);
	EXPECT_EQ("P1_START", maps[1]["target"]);
	EXPECT_EQ("313",      maps[1]["trigger"]);
	EXPECT_EQ("Blue",     maps[1]["color"]);
	EXPECT_EQ("Combine",  maps[1]["filter"]);

	EXPECT_EQ("Element",    maps[2]["type"]);
	EXPECT_EQ("P1_BUTTON1", maps[2]["target"]);
	EXPECT_EQ("315",        maps[2]["trigger"]);
	EXPECT_EQ("Yellow",     maps[2]["color"]);
	EXPECT_EQ("Normal",     maps[2]["filter"]);
}

TEST_F(InputFileTest, SecondSourceMapsAreProcessed) {
	const string mapName(Defaults::createCommonUniqueId({baseId("inputMulti"), "1", COLLECTION_INPUT_MAPS}));

	auto& maps = inputMulti->getData(mapName);
	ASSERT_EQ(1, maps.size());

	EXPECT_EQ("Element",  maps[0]["type"]);
	EXPECT_EQ("P2_START", maps[0]["target"]);
	EXPECT_EQ("313",      maps[0]["trigger"]);
	EXPECT_EQ("Red",      maps[0]["color"]);
	EXPECT_EQ("Combine",  maps[0]["filter"]);
}

TEST_F(InputFileTest, SingleSourceInputIsLoaded) {
	auto& inputData = inputSingle->getData(COLLECTION_INPUT);
	ASSERT_FALSE(inputData.empty());
	EXPECT_EQ("Mame", inputData[0][NAME]);
	EXPECT_EQ("inputSingle", inputSingle->getFilename());
}

TEST_F(InputFileTest, SingleSourceMapsAreProcessed) {
	const string mapName(Defaults::createCommonUniqueId({baseId("inputSingle"), "0", COLLECTION_INPUT_MAPS}));

	auto& maps = inputSingle->getData(mapName);
	ASSERT_EQ(3, maps.size());

	EXPECT_EQ("Group",        maps[0]["type"]);
	EXPECT_EQ("player 1",     maps[0]["target"]);
	EXPECT_EQ("Left_Gun",     maps[0]["trigger"]);
	EXPECT_EQ("Red",          maps[0]["color"]);
	EXPECT_EQ("Normal",       maps[0]["filter"]);

	EXPECT_EQ("Group",        maps[1]["type"]);
	EXPECT_EQ("player 1",     maps[1]["target"]);
	EXPECT_EQ("Left_Green",   maps[1]["trigger"]);
	EXPECT_EQ("Green",        maps[1]["color"]);
	EXPECT_EQ("Normal",       maps[1]["filter"]);

	EXPECT_EQ("Group",        maps[2]["type"]);
	EXPECT_EQ("player 1",     maps[2]["target"]);
	EXPECT_EQ("Left_Flash_1", maps[2]["trigger"]);
	EXPECT_EQ("Yellow",       maps[2]["color"]);
	EXPECT_EQ("Combine",      maps[2]["filter"]);
}

TEST_F(InputFileTest, MissingAttributesHandling) {
	EXPECT_THROW(
		InputFile(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputMalformed.xml", nullptr),
		Message
	);
}

TEST_F(InputFileTest, RootInfoIsPopulated) {
	const auto& rootInfo = inputMulti->getRootInfo();

	EXPECT_EQ(PACKAGE_DATA_VERSION, rootInfo.version);
	EXPECT_EQ("Input",   rootInfo.type);
	EXPECT_FALSE(rootInfo.attributes.empty());
	EXPECT_EQ("Credits", rootInfo.attributes.at(NAME));
}
