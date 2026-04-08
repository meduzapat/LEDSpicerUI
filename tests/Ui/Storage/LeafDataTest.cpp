/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LeafDataTest.cpp
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
#include "Storage/RestrictorMap.hpp"
#include "Storage/InputMapLink.hpp"
#include "Storage/Process.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;

// RestrictorMap -------------------------------------------------------

TEST(RestrictorMapTest, CreateUniqueId) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	EXPECT_EQ(Defaults::createCommonUniqueId({"1", "1"}), r.createUniqueId());
}

TEST(RestrictorMapTest, CreatePrettyNameBasic) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "2"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	EXPECT_EQ("Player 1 Joystick 2", r.createPrettyName());
}

TEST(RestrictorMapTest, CreatePrettyNameWithInterface) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, "3"}};
	RestrictorMap r(data);
	EXPECT_NE(string::npos, r.createPrettyName().find("interface 3"));
}

TEST(RestrictorMapTest, ToXMLSelfClosing) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	const string xml(r.toXML());
	EXPECT_NE(string::npos, xml.find("<map"));
	EXPECT_NE(string::npos, xml.find("/>"));
}

TEST(RestrictorMapTest, CssClass) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	EXPECT_EQ("RestrictorMapBoxButton", r.getCssClass());
}

// InputMapLink --------------------------------------------------------

TEST(InputMapLinkTest, CreateUniqueIdFromIdField) {
	StringUMap data{
		{ID,   "myid"},
		{NAME, string("305") + FIELD_SEPARATOR + "Element P1_BUTTON1"}
	};
	InputMapLink link(data);
	EXPECT_EQ("myid", link.createUniqueId());
}

TEST(InputMapLinkTest, CreatePrettyNameParsesName) {
	StringUMap data{
		{ID,   "id"},
		{NAME, string("305") + FIELD_SEPARATOR + "Element P1_BUTTON1"}
	};
	InputMapLink link(data);
	EXPECT_NE(string::npos, link.createPrettyName().find("Element P1_BUTTON1"));
}

TEST(InputMapLinkTest, CssClass) {
	StringUMap data{{ID, "x"}, {NAME, string("1") + FIELD_SEPARATOR + "Element X"}};
	InputMapLink link(data);
	EXPECT_EQ("LinkBoxButton", link.getCssClass());
}

// Process -------------------------------------------------------------

TEST(ProcessTest, CreatePrettyName) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_NE(string::npos, p.createPrettyName().find("mame"));
	EXPECT_NE(string::npos, p.createPrettyName().find("Linux"));
}

TEST(ProcessTest, CreateUniqueIdFromPrimaryKey) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_EQ("mame", p.createUniqueId());
}

TEST(ProcessTest, ToXMLContainsMapTag) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_NE(string::npos, p.toXML().find("<map"));
}

TEST(ProcessTest, CssClass) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_EQ("ProcessBoxButton", p.getCssClass());
}
