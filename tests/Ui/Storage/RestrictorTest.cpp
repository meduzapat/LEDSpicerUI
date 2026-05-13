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
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;

class RestrictorTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(RestrictorTest, TestFunctionality) {

	Values data {{NAME, "ServoStik"}, {ID, "1"}, {PORT, ""}};
	Restrictor r {data};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_RESTRICTOR_BOX_BUTTON, r.getCssClass());
	EXPECT_EQ(TYPE_RESTRICTOR, r.getXmlTag());
	EXPECT_EQ(CollectionHandler::getInstance(COLLECTION_RESTRICTORS), r.getCollectionHandler());

	// createPrettyName: basic label, then includes ID when non-default.
	EXPECT_NE(string::npos, r.createPrettyName().find("ServoStik"));
	r.setValue(ID, "3");
	EXPECT_NE(string::npos, r.createPrettyName().find("Id: 3"));
	r.setValue(ID, "1");

	// createUniqueId: matches hardware identity helper.
	const Values values {r.copyValues()};
	EXPECT_EQ(Defaults::createHardwareUniqueId(values, false), r.createUniqueId());

	// Child collection keyed as COLLECTION_RESTRICTOR_MAPS.
	EXPECT_NE(nullptr, r.getChild(COLLECTION_RESTRICTOR_MAPS));

	// toXML.
	const string xml(r.toXML());
	EXPECT_EQ("<restrictor name=\"ServoStik\" boardId=\"1\"/>\n", xml);

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
