/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogPromptTest.cpp
 * @since     Jun 12, 2026
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
#include "DialogPrompt.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Ui;

TEST(DialogPromptTest, BuildPlayerControlName) {
	// Full form with WAYS modifier.
	EXPECT_EQ("P1_JOYSTICK1_8WAYS",
	          DialogPrompt::buildPlayerControlName("1", "JOYSTICK", "1", "8WAYS"));
	// Button, no ways.
	EXPECT_EQ("P2_BUTTON4",
	          DialogPrompt::buildPlayerControlName("2", "BUTTON", "4", emptyString));
	// START — no index, no ways even if passed.
	EXPECT_EQ("P1_START",
	          DialogPrompt::buildPlayerControlName("1", "START", emptyString, emptyString));
	EXPECT_EQ("P1_START",
	          DialogPrompt::buildPlayerControlName("1", "START", "5", "anything"));
	// COIN — same rule as START.
	EXPECT_EQ("P3_COIN",
	          DialogPrompt::buildPlayerControlName("3", "COIN", emptyString, emptyString));
	// Player missing.
	EXPECT_EQ(emptyString,
	          DialogPrompt::buildPlayerControlName(emptyString, "JOYSTICK", "1", emptyString));
	// Type missing.
	EXPECT_EQ(emptyString,
	          DialogPrompt::buildPlayerControlName("1", emptyString, "1", emptyString));
	// Index required and missing.
	EXPECT_EQ(emptyString,
	          DialogPrompt::buildPlayerControlName("1", "BUTTON", emptyString, emptyString));
}

TEST(DialogPromptTest, BuildCabinetItemName) {
	EXPECT_EQ("FLOOR",     DialogPrompt::buildCabinetItemName("FLOOR", emptyString));
	EXPECT_EQ("FLOOR2",    DialogPrompt::buildCabinetItemName("FLOOR", "2"));
	EXPECT_EQ(emptyString, DialogPrompt::buildCabinetItemName(emptyString, "1"));
	EXPECT_EQ(emptyString, DialogPrompt::buildCabinetItemName(emptyString, emptyString));
}
