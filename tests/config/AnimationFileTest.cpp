/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AnimationFileTest.cpp
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
#include "config/AnimationFile.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;

class AnimationFileTest : public ::testing::Test {

protected:

	void SetUp() override {
		animationFull = std::make_unique<AnimationFile>(
			PACKAGE_SAMPLES_DIR "data/" + PATH_ANIMATION + "animationFull.xml",
			nullptr
		);
	}

	void TearDown() override {
		animationFull.reset();
	}

	std::unique_ptr<AnimationFile> animationFull;

	/// Collection key for root-level animation files.
	static string animationsKey() {
		return Defaults::createCommonUniqueId({"", COLLECTION_ANIMATIONS});
	}

	/// Per-file actors key.
	static string actorsKey(const string& filename) {
		return Defaults::createCommonUniqueId({
			Defaults::createCommonUniqueId({"", filename}),
			COLLECTION_ACTORS
		});
	}
};

TEST_F(AnimationFileTest, RootInfoIsPopulated) {
	const auto& rootInfo = animationFull->getRootInfo();
	EXPECT_EQ(PACKAGE_DATA_VERSION, rootInfo.getValue("version"));
	EXPECT_EQ("Animation",          rootInfo.getValue("type"));
}

TEST_F(AnimationFileTest, FilenameIsExtracted) {
	EXPECT_EQ("animationFull", animationFull->getFilename());
}

TEST_F(AnimationFileTest, AnimationEntryIsCreated) {
	auto& animationData = animationFull->getData(animationsKey());
	ASSERT_FALSE(animationData.empty());
	EXPECT_EQ("animationFull", animationData[0].getValue(FILENAME));
}

TEST_F(AnimationFileTest, AllActorsAreExtracted) {
	auto& actors = animationFull->getData(actorsKey("animationFull"));
	ASSERT_EQ(4u, actors.size());

	EXPECT_EQ("Filler",     actors[0].getValue(TYPE));
	EXPECT_EQ("Serpentine", actors[1].getValue(TYPE));
	EXPECT_EQ("Gradient",   actors[2].getValue(TYPE));
	EXPECT_EQ("AlsaAudio",  actors[3].getValue(TYPE));
}

TEST_F(AnimationFileTest, FillerAttributesAreParsed) {
	auto& actor = animationFull->getData(actorsKey("animationFull"))[0];
	EXPECT_EQ("All",     actor.getValue(ACTOR_GROUP));
	EXPECT_EQ("Blue",    actor.getValue(COLOR));
	EXPECT_EQ("Combine", actor.getValue(FILTER));
	EXPECT_EQ("Forward", actor.getValue(DIRECTION));
	EXPECT_EQ(HUMAN_TRUE, actor.getValue(BOUNCER));
	EXPECT_EQ(HUMAN_NORMAL, actor.getValue(MODE));
}

TEST_F(AnimationFileTest, SerpentineTailAttributesAreParsed) {
	auto& actor = animationFull->getData(actorsKey("animationFull"))[1];
	EXPECT_EQ("Backward", actor.getValue(DIRECTION));
	EXPECT_EQ("4",        actor.getValue(END_TIME));
	EXPECT_EQ("4",        actor.getValue(RESTART_TIME));
	EXPECT_EQ("3",        actor.getValue(TAIL_LENGTH));
	EXPECT_EQ("DarkRed",  actor.getValue(TAIL_COLOR));
	EXPECT_EQ("75",       actor.getValue(TAIL_INTENSITY));
}

TEST_F(AnimationFileTest, GradientMultiColorsArePreserved) {
	auto& actor = animationFull->getData(actorsKey("animationFull"))[2];
	EXPECT_EQ("Red,Green,Blue", actor.getValue(COLORS));
	EXPECT_EQ("Cyclic", actor.getValue(MODE));
	EXPECT_EQ("10",     actor.getValue(TONES));
}

TEST_F(AnimationFileTest, AudioPaletteAttributesAreParsed) {
	auto& actor = animationFull->getData(actorsKey("animationFull"))[3];
	EXPECT_EQ("VuMeter", actor.getValue(MODE));
	EXPECT_EQ("Both",    actor.getValue(CHANNEL));
	EXPECT_EQ("Off",     actor.getValue(OFF));
	EXPECT_EQ("Green",   actor.getValue(LOW));
	EXPECT_EQ("Yellow",  actor.getValue(MID));
	EXPECT_EQ("Red",     actor.getValue(HIGH));
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
