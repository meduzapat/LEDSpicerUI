/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceTest.cpp
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
#include "Storage/Device.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;

class DeviceTest : public ::testing::Test {
protected:
	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

// Empty fieldsData returns empty uniqueId.
TEST_F(DeviceTest, CreateUniqueIdEmptyWhenNoFields) {
	StringUMap empty;
	Device d(empty);
	EXPECT_TRUE(d.createUniqueId().empty());
}

// Non-empty data returns non-empty uniqueId.
TEST_F(DeviceTest, CreateUniqueIdNonEmpty) {
	StringUMap data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d(data);
	EXPECT_FALSE(d.createUniqueId().empty());
}

// createPrettyName for a simple non-ID, non-serial device.
TEST_F(DeviceTest, CreatePrettyNameSimpleDevice) {
	StringUMap data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d(data);
	EXPECT_NE(string::npos, d.createPrettyName().find("Raspberry Pi GPIO"));
}

// createPrettyName includes ID for ID-user devices.
TEST_F(DeviceTest, CreatePrettyNameIncludesId) {
	StringUMap data{{NAME, "UltimarcPacDrive"}, {ID, "2"}, {PORT, ""}};
	Device d(data);
	EXPECT_NE(string::npos, d.createPrettyName().find("Id: 2"));
}

// Child collection keyed as COLLECTION_ELEMENT.
TEST_F(DeviceTest, HasElementChild) {
	StringUMap data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d(data);
	EXPECT_NE(nullptr, d.getChild(COLLECTION_ELEMENT));
}

// getCssClass.
TEST_F(DeviceTest, CssClass) {
	StringUMap data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d(data);
	EXPECT_EQ("DeviceBoxButton", d.getCssClass());
}

// toXML contains device tag.
TEST_F(DeviceTest, ToXMLStructure) {
	StringUMap data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d(data);
	const string xml(d.toXML());
	EXPECT_NE(string::npos, xml.find("<device"));
	EXPECT_NE(string::npos, xml.find("</device>"));
}

// wipe clears fieldsData, snap stays clean.
TEST_F(DeviceTest, WipeClearsFields) {
	StringUMap data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d(data);
	d.wipe();
	EXPECT_TRUE(d.getValues()->empty());
}
