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
		inputMulti  = std::make_unique<InputFile>(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputMulti.xml",  nullptr);
		inputSingle = std::make_unique<InputFile>(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputSingle.xml", nullptr);
	}

	void TearDown() override {
		inputMulti.reset();
		inputSingle.reset();
	}

	std::unique_ptr<InputFile> inputMulti;
	std::unique_ptr<InputFile> inputSingle;

	/*
	 * Builds the base collection key for a file at root level (empty relPath).
	 * Matches InputFile::InputFile() → createCommonUniqueId({"", filename}).
	 */
	static string baseId(const string& filename) {
		return Defaults::createCommonUniqueId({"", filename});
	}

	/*
	 * Builds the inputs collection key for root-level files.
	 * Matches InputFile::InputFile() → createCommonUniqueId({"", COLLECTION_INPUTS}).
	 */
	static string inputsKey() {
		return Defaults::createCommonUniqueId({"", COLLECTION_INPUTS});
	}
};

TEST_F(InputFileTest, MultiSourceInputIsLoaded) {
	auto& inputData = inputMulti->getData(inputsKey());
	ASSERT_FALSE(inputData.empty());
	ASSERT_NE(0u, inputData[0].getSize());

	EXPECT_EQ("Credits",    inputData[0].getValue(NAME));
	EXPECT_EQ(HUMAN_NORMAL, inputData[0].getValue(SPEED));
	EXPECT_EQ("0,1|2,3",    inputData[0].getValue(LINKED_ITEMS));
	EXPECT_EQ("Single",     inputData[0].getValue(MODE));
	EXPECT_EQ(HUMAN_TRUE,   inputData[0].getValue(ONCE));
	EXPECT_EQ(HUMAN_FALSE,  inputData[0].getValue(ALWAYS_ON));
	EXPECT_EQ("1",          inputData[0].getValue(COINS_CREDIT));

	EXPECT_EQ("inputMulti", inputMulti->getFilename());
}

TEST_F(InputFileTest, SourcesAreExtracted) {
	string eventName(Defaults::createCommonUniqueId({baseId("inputMulti"), COLLECTION_INPUT_SOURCES}));

	EXPECT_EQ("hardware1", inputMulti->getData(eventName)[0].getValue(SOURCE));
	EXPECT_EQ("hardware2", inputMulti->getData(eventName)[1].getValue(SOURCE));
}

TEST_F(InputFileTest, FirstSourceMapsAreProcessed) {
	string mapName(Defaults::createCommonUniqueId({baseId("inputMulti"), "0", COLLECTION_INPUT_MAPS}));

	auto& maps = inputMulti->getData(mapName);
	ASSERT_EQ(3, maps.size());

	EXPECT_EQ("Element", maps[0].getValue(TYPE));
	EXPECT_EQ("P1_COIN", maps[0].getValue(TARGET));
	EXPECT_EQ("309",     maps[0].getValue(TRIGGER));
	EXPECT_EQ("White",   maps[0].getValue(COLOR));
	EXPECT_EQ("Combine", maps[0].getValue(FILTER));

	EXPECT_EQ("Element",  maps[1].getValue(TYPE));
	EXPECT_EQ("P1_START", maps[1].getValue(TARGET));
	EXPECT_EQ("313",      maps[1].getValue(TRIGGER));
	EXPECT_EQ("Blue",     maps[1].getValue(COLOR));
	EXPECT_EQ("Combine",  maps[1].getValue(FILTER));

	EXPECT_EQ("Element",    maps[2].getValue(TYPE));
	EXPECT_EQ("P1_BUTTON1", maps[2].getValue(TARGET));
	EXPECT_EQ("315",        maps[2].getValue(TRIGGER));
	EXPECT_EQ("Yellow",     maps[2].getValue(COLOR));
	EXPECT_EQ("Normal",     maps[2].getValue(FILTER));
}

TEST_F(InputFileTest, SecondSourceMapsAreProcessed) {
	string mapName(Defaults::createCommonUniqueId({baseId("inputMulti"), "1", COLLECTION_INPUT_MAPS}));

	auto& maps = inputMulti->getData(mapName);
	ASSERT_EQ(1, maps.size());

	EXPECT_EQ("Element",  maps[0].getValue(TYPE));
	EXPECT_EQ("P2_START", maps[0].getValue(TARGET));
	EXPECT_EQ("313",      maps[0].getValue(TRIGGER));
	EXPECT_EQ("Red",      maps[0].getValue(COLOR));
	EXPECT_EQ("Combine",  maps[0].getValue(FILTER));
}

TEST_F(InputFileTest, SingleSourceInputIsLoaded) {
	auto& inputData = inputSingle->getData(inputsKey());
	ASSERT_FALSE(inputData.empty());
	EXPECT_EQ("Mame", inputData[0].getValue(NAME));
	EXPECT_EQ("inputSingle", inputSingle->getFilename());
}

TEST_F(InputFileTest, SingleSourceMapsAreProcessed) {
	string mapName(Defaults::createCommonUniqueId({baseId("inputSingle"), "0", COLLECTION_INPUT_MAPS}));

	auto& maps = inputSingle->getData(mapName);
	ASSERT_EQ(3, maps.size());

	EXPECT_EQ("Group",        maps[0].getValue(TYPE));
	EXPECT_EQ("player 1",     maps[0].getValue(TARGET));
	EXPECT_EQ("Left_Gun",     maps[0].getValue(TRIGGER));
	EXPECT_EQ("Red",          maps[0].getValue(COLOR));
	EXPECT_EQ("Normal",       maps[0].getValue(FILTER));

	EXPECT_EQ("Group",        maps[1].getValue(TYPE));
	EXPECT_EQ("player 1",     maps[1].getValue(TARGET));
	EXPECT_EQ("Left_Green",   maps[1].getValue(TRIGGER));
	EXPECT_EQ("Green",        maps[1].getValue(COLOR));
	EXPECT_EQ("Normal",       maps[1].getValue(FILTER));

	EXPECT_EQ("Group",        maps[2].getValue(TYPE));
	EXPECT_EQ("player 1",     maps[2].getValue(TARGET));
	EXPECT_EQ("Left_Flash_1", maps[2].getValue(TRIGGER));
	EXPECT_EQ("Yellow",       maps[2].getValue(COLOR));
	EXPECT_EQ("Combine",      maps[2].getValue(FILTER));
}

TEST_F(InputFileTest, MissingAttributesHandling) {
	EXPECT_THROW(
		InputFile(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputMalformed.xml", nullptr),
		Message
	);
}

TEST_F(InputFileTest, RootInfoIsPopulated) {
	const auto& rootInfo = inputMulti->getRootInfo();

	EXPECT_EQ(PACKAGE_DATA_VERSION, rootInfo.getValue("version"));
	EXPECT_EQ("Input",   rootInfo.getValue("type"));
	EXPECT_NE(0u,        rootInfo.getSize());
	EXPECT_EQ("Credits", rootInfo.getValue(NAME));
}

TEST_F(InputFileTest, LinkedTriggersAreExtracted) {
	string imlKey{Defaults::createCommonUniqueId({baseId("inputMulti"), COLLECTION_INPUT_LINKMAPS})};
	auto& imlData{inputMulti->getData(imlKey)};
	ASSERT_EQ(2u, imlData.size());
	EXPECT_EQ("0,1", imlData[0].getValue(LINKED_ITEMS));
	EXPECT_EQ("2,3", imlData[1].getValue(LINKED_ITEMS));
}

TEST_F(InputFileTest, NonLinkedInputHasNoIMLData) {
	// inputSingle is Mame type — no INPUT_LINKED_MAPS flag.
	string imlKey{Defaults::createCommonUniqueId({baseId("inputSingle"), COLLECTION_INPUT_LINKMAPS})};
	EXPECT_TRUE(inputSingle->getData(imlKey).empty());
}
