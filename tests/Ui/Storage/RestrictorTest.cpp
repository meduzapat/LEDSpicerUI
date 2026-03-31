/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RestrictorTest.cpp
 * @since     Mar 30, 2026
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
#include "Storage/Restrictor.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;

class RestrictorTest : public ::testing::Test {
protected:
	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

// Empty fieldsData returns empty uniqueId.
TEST_F(RestrictorTest, CreateUniqueIdEmptyWhenNoFields) {
	StringUMap empty;
	Restrictor r(empty);
	EXPECT_TRUE(r.createUniqueId().empty());
}

// Non-empty data returns non-empty uniqueId.
TEST_F(RestrictorTest, CreateUniqueIdNonEmpty) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r(data);
	EXPECT_FALSE(r.createUniqueId().empty());
}

// createPrettyName for a simple restrictor.
TEST_F(RestrictorTest, CreatePrettyNameSimple) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r(data);
	EXPECT_NE(string::npos, r.createPrettyName().find("ServoStik"));
}

// createPrettyName includes ID for ID-user restrictors.
TEST_F(RestrictorTest, CreatePrettyNameIncludesId) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "3"}, {PORT, ""}};
	Restrictor r(data);
	EXPECT_NE(string::npos, r.createPrettyName().find("Id: 3"));
}

// Child collection keyed as COLLECTION_RESTRICTOR_MAP.
TEST_F(RestrictorTest, HasRestrictorMapChild) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r(data);
	EXPECT_NE(nullptr, r.getChild(COLLECTION_RESTRICTOR_MAP));
}

// getCssClass.
TEST_F(RestrictorTest, CssClass) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r(data);
	EXPECT_EQ("RestrictorBoxButton", r.getCssClass());
}

// toXML contains restrictor tag.
TEST_F(RestrictorTest, ToXMLStructure) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r(data);
	const string xml(r.toXML());
	EXPECT_NE(string::npos, xml.find("<restrictor"));
	EXPECT_NE(string::npos, xml.find("</restrictor>"));
}

// wipe clears fieldsData.
TEST_F(RestrictorTest, WipeClearsFields) {
	StringUMap data{{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r(data);
	r.wipe();
	EXPECT_TRUE(r.getValues()->empty());
}
