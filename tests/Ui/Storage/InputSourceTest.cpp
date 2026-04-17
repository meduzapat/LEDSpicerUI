/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSourceTest.cpp
 * @since     Apr 2026
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
#include "Storage/InputSource.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;

class InputSourceTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

// Constructor sets UID property.
TEST_F(InputSourceTest, ConstructorSetsUid) {
	StringUMap data{{SOURCE, "hardware1"}};
	InputSource src(data, "owner_1");
	EXPECT_FALSE(src.getProperties().getValue(UID).empty());
}

// Constructor stores ownerId in PID property.
TEST_F(InputSourceTest, ConstructorSetsPid) {
	StringUMap data{{SOURCE, "hardware1"}};
	InputSource src(data, "owner_1");
	EXPECT_EQ("owner_1", src.getProperties().getValue(PID));
}

// Each instance gets a distinct UID.
TEST_F(InputSourceTest, MultipleInstancesHaveDistinctUids) {
	StringUMap d1{{SOURCE, "hw1"}}, d2{{SOURCE, "hw2"}};
	InputSource s1(d1, "owner_1");
	InputSource s2(d2, "owner_1");
	EXPECT_NE(s1.getProperties().getValue(UID), s2.getProperties().getValue(UID));
}

// createUniqueId = PID + SOURCE.
TEST_F(InputSourceTest, CreateUniqueIdCombinesPidAndSource) {
	StringUMap data{{SOURCE, "hardware1"}};
	InputSource src(data, "owner_1");
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"owner_1", "hardware1"}),
		src.createUniqueId()
	);
}

// createUniqueId with empty source.
TEST_F(InputSourceTest, CreateUniqueIdEmptySource) {
	StringUMap data;
	InputSource src(data, "owner_1");
	EXPECT_EQ(
		Defaults::createCommonUniqueId({"owner_1", ""}),
		src.createUniqueId()
	);
}

// getCollectionHandler scopes by PID.
TEST_F(InputSourceTest, CollectionHandlerScopedByPid) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	auto ch{src.getCollectionHandler()};
	ASSERT_NE(nullptr, ch);
	// A different owner produces a different handler.
	StringUMap data2{{SOURCE, "hw1"}};
	InputSource src2(data2, "owner_2");
	EXPECT_NE(src.getCollectionHandler(), src2.getCollectionHandler());
}

// getCssClass and getXmlTag.
TEST_F(InputSourceTest, CssClass) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	EXPECT_EQ("InputSourceBoxButton", src.getCssClass());
}

TEST_F(InputSourceTest, XmlTag) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	EXPECT_EQ("maps", src.getXmlTag());
}

// createPrettyName reads NAME property set externally.
TEST_F(InputSourceTest, CreatePrettyNameFromProperty) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	src.getProperties().setValue(NAME, "Joystick 1");
	EXPECT_EQ("Joystick 1", src.createPrettyName());
}

// createTooltip reflects map count.
TEST_F(InputSourceTest, CreateTooltipReflectsMapCount) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	src.getProperties().setValue(NAME, "hw1");
	string tooltip(src.createTooltip());
	EXPECT_NE(string::npos, tooltip.find("0"));
}

// shouldSerialize suppresses SOURCE when empty.
TEST_F(InputSourceTest, ShouldSerializeEmptySourceSuppressed) {
	StringUMap data;
	InputSource src(data, "owner_1");
	EXPECT_EQ(string::npos, src.toXML().find(SOURCE));
}

// shouldSerialize includes SOURCE when non-empty.
TEST_F(InputSourceTest, ShouldSerializeNonEmptySourceIncluded) {
	StringUMap data{{SOURCE, "hardware1"}};
	InputSource src(data, "owner_1");
	EXPECT_NE(string::npos, src.toXML().find(SOURCE));
}

// Has COLLECTION_INPUT_MAPS child collection.
TEST_F(InputSourceTest, HasInputMapsChild) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	EXPECT_NE(nullptr, src.getChild(COLLECTION_INPUT_MAPS));
}

// wipe clears values and clearSnap contract.
TEST_F(InputSourceTest, WipeClearsValues) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	src.wipe();
	EXPECT_TRUE(src.getValues()->empty());
}

// snapshot / revert round-trip.
TEST_F(InputSourceTest, SnapshotAndRevert) {
	StringUMap data{{SOURCE, "hw1"}};
	InputSource src(data, "owner_1");
	const StringUMap before(*src.getValues());
	src.snapshot();
	EXPECT_TRUE(src.getValues()->empty());
	src.revert();
	EXPECT_EQ(before, *src.getValues());
}
