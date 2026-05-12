/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonTest.cpp
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

#include "MockData.hpp"
#include "Storage/BoxButton.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using LEDSpicerUI::Test::Mocks::MockData;

class BoxButtonTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(BoxButtonTest, RegistrationMechanics) {

	Values d {{"name", "Item"}};
	auto data {new MockData {d}};
	auto ch {data->getCollectionHandler()};
	EXPECT_FALSE(ch->isSet(data));

	{
		// Constructor registers the Data into its collection.
		BoxButton btn(data);
		EXPECT_TRUE(ch->isSet(data));

		// getData() returns the owned pointer.
		EXPECT_EQ(data, btn.getData());

		// sync() reflects createPrettyName().
		EXPECT_EQ("Item", btn.getLabel()->get_text());
		data->setValue("name", "Item2");
		btn.sync();
		EXPECT_EQ("Item2", btn.getLabel()->get_text());
		// Simulate DialogForm replace.
		ch->replace(data, "Item");
	}
	// Destructor unregisters the Data from its collection.
	EXPECT_FALSE(ch->isIdSet("Item"));
}

// Move constructor nulls the source data pointer.
TEST_F(BoxButtonTest, MoveConstructorNullsSource) {
	Values d {{"name", "Item"}};
	auto data {new MockData {d}};
	BoxButton src(data);
	BoxButton dst(std::move(src));
	EXPECT_EQ(nullptr, src.getData());
	EXPECT_EQ(data,    dst.getData());
}

// Destroying a moved-from BoxButton does not crash or double-unregister.
TEST_F(BoxButtonTest, MovedFromDestructionSafe) {
	Values d {{"name", "Item"}};
	auto data {new MockData {d}};
	auto ch {CollectionHandler::getInstance("bb_test")};
	auto src {new BoxButton(data)};
	BoxButton dst(std::move(*src));
	EXPECT_NO_FATAL_FAILURE(delete src);
	// dst still owns the data and it is still registered.
	EXPECT_TRUE(ch->isSet(data));
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
