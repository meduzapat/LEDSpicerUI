/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProfileFileTest.cpp
 * @since     May 2026
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
#include "config/ProfileFile.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;

class ProfileFileTest : public ::testing::Test {

protected:

	void SetUp() override {
		profile = std::make_unique<ProfileFile>(
			PACKAGE_SAMPLES_DIR "data/" + PATH_PROFILE + "profile.xml",
			nullptr
		);
	}

	void TearDown() override {
		profile.reset();
	}

	std::unique_ptr<ProfileFile> profile;

	/// Collection key for root-level profile files.
	static string profilesKey() {
		return Defaults::createCommonUniqueId({"", COLLECTION_PROFILES});
	}

	/// Per-file section key (e.g. alwaysOnElements) under the profile's baseId.
	static string sectionKey(const string& filename, const string& collection) {
		return Defaults::createCommonUniqueId({
			Defaults::createCommonUniqueId({"", filename}),
			collection
		});
	}
};

TEST_F(ProfileFileTest, RootInfoIsPopulated) {
	const auto& rootInfo = profile->getRootInfo();
	EXPECT_EQ(PACKAGE_DATA_VERSION, rootInfo.getValue("version"));
	EXPECT_EQ("Profile",            rootInfo.getValue("type"));
	EXPECT_EQ("Off",                rootInfo.getValue(BACKGROUND_COLOR));
}

TEST_F(ProfileFileTest, FilenameIsExtracted) {
	EXPECT_EQ("profile", profile->getFilename());
}

TEST_F(ProfileFileTest, ProfileEntryIsCreated) {
	auto& profileData = profile->getData(profilesKey());
	ASSERT_FALSE(profileData.empty());
	EXPECT_EQ("profile", profileData[0].getValue(FILENAME));
	EXPECT_EQ("Off",     profileData[0].getValue(BACKGROUND_COLOR));
}

TEST_F(ProfileFileTest, BackgroundColorDefaultsToOffWhenAbsent) {
	// A minimal profile file would still get the default.
	// Inline check against the constant the parser uses.
	EXPECT_EQ(HUMAN_OFF, DEFAULT_PROFILE_BACKGROUND_COLOR);
}

TEST_F(ProfileFileTest, AlwaysOnElementsAreExtracted) {
	auto& elements = profile->getData(sectionKey("profile", COLLECTION_PROFILE_ELEMENTS));
	ASSERT_EQ(3u, elements.size());
	EXPECT_EQ("P1_BUTTON_1", elements[0].getValue(NAME));
	EXPECT_EQ("White",       elements[0].getValue(COLOR));
	EXPECT_EQ("P1_BUTTON_2", elements[1].getValue(NAME));
	EXPECT_EQ("P1_BUTTON_3", elements[2].getValue(NAME));
}

TEST_F(ProfileFileTest, AlwaysOnGroupsAreExtracted) {
	auto& groups = profile->getData(sectionKey("profile", COLLECTION_PROFILE_GROUPS));
	ASSERT_EQ(1u, groups.size());
	EXPECT_EQ("CONTROLS", groups[0].getValue(NAME));
	EXPECT_EQ("Gray",     groups[0].getValue(COLOR));
}

TEST_F(ProfileFileTest, AnimationsAreExtracted) {
	auto& animations = profile->getData(sectionKey("profile", COLLECTION_PROFILE_ANIMATIONS));
	ASSERT_EQ(2u, animations.size());
	EXPECT_EQ("cabinetSoundAware", animations[0].getValue(NAME));
	EXPECT_EQ(TEST_STR,            animations[1].getValue(NAME));
}

TEST_F(ProfileFileTest, InputsAreExtracted) {
	auto& inputs = profile->getData(sectionKey("profile", COLLECTION_PROFILE_INPUTS));
	ASSERT_EQ(1u, inputs.size());
	EXPECT_EQ("ControlPanel", inputs[0].getValue(NAME));
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
