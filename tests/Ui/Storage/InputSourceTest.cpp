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
#include "Storage/InputMap.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;

class InputSourceTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(InputSourceTest, TestFunctionality) {

	Values data {{SOURCE, "hardware1"}};
	InputSource src {data, "owner_1"};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_INPUT_SOURCE_BOX_BUTTON, src.getCssClass());
	EXPECT_EQ(TYPE_MAPS,                   src.getXmlTag());
	EXPECT_NE(nullptr,                     src.getCollectionHandler());

	// getCollectionHandler scoped by owner.
	Values data2 {{SOURCE, "hardware1"}};
	InputSource src2 {data2, "owner_2"};
	EXPECT_NE(src.getCollectionHandler(), src2.getCollectionHandler());

	// createUniqueId combines PID and SOURCE.
	EXPECT_EQ(Defaults::createCommonUniqueId({"owner_1", "hardware1"}), src.createUniqueId());

	// createPrettyName reads NAME from properties.
	src.getProperties().setValue(NAME, "Joystick 1");
	EXPECT_EQ("Joystick 1", src.createPrettyName());

	// shouldSerialize: SOURCE suppressed when empty, included when non-empty.
	// Must be checked before adding maps (InputMap with null link crashes toXML).
	Values empty;
	InputSource srcEmpty {empty, "owner_1"};
	EXPECT_EQ(string::npos, srcEmpty.toXML().find(SOURCE));
	EXPECT_NE(string::npos, src.toXML().find(SOURCE));

	// createTooltip with zero maps.
	EXPECT_NE(string::npos, src.createTooltip().find("0"));

	// createTooltip with maps present.
	auto mapCh {src.getChild(COLLECTION_INPUT_MAPS)};
	Values mapData {{TYPE, "button"}, {TARGET, "p1_b1"}, {TRIGGER, "press"}, {COLOR, "red"}, {FILTER, ""}};
	mapCh->create(new InputMap(mapData, nullptr));
	EXPECT_NE(string::npos, src.createTooltip().find("1"));

	// Child collection keyed as COLLECTION_INPUT_MAPS.
	EXPECT_NE(nullptr, mapCh);

	// Dependency registration: COLLECTION_ELEMENTS and COLLECTION_GROUPS are watched.
	EXPECT_NE(nullptr, CollectionHandler::getInstance(COLLECTION_ELEMENTS));
	EXPECT_NE(nullptr, CollectionHandler::getInstance(COLLECTION_GROUPS));

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
