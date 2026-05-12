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
using LEDSpicerUI::Values;

class DeviceTest : public ::testing::Test {
protected:
	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(DeviceTest, CreatePrettyNameSimpleDevice) {

	Values data {{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d {data};

	// uniqueId.
	EXPECT_FALSE(d.createUniqueId().empty());
	// createPrettyName.
	EXPECT_NE(string::npos, d.createPrettyName().find("Raspberry Pi GPIO"));
	// getCssClass.
	EXPECT_EQ("DeviceBoxButton", d.getCssClass());
}
TEST_F(DeviceTest, HasElementChild) {
	Values data{{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d {data};
	// Child collection keyed as COLLECTION_ELEMENTS.
	EXPECT_NE(nullptr, d.getChild(COLLECTION_ELEMENTS));
}

TEST_F(DeviceTest, ToXMLStructure) {
	Values data {{NAME, "RaspberryPi"}, {ID, "1"}, {PORT, ""}};
	Device d {data};
	const string xml(d.toXML());
	EXPECT_EQ("<device name=\"RaspberryPi\"/>\n", xml);
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
