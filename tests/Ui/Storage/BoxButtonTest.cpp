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

#include <gtest/gtest.h>
#include "Storage/BoxButton.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;

class TestData : public Data {

public:

	TestData(StringUMap& d) noexcept : Data(d) {}
	const string& getCssClass() const noexcept override { return "test"; }
	const string& getXmlTag()   const noexcept override { return "test"; }
	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance("bb_test");
	}
};

class BoxButtonTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

// Constructor registers the Data into its collection.
TEST_F(BoxButtonTest, ConstructorRegisters) {
	StringUMap d{{"name", "Item"}};
	auto data{new TestData(d)};
	auto ch{CollectionHandler::getInstance("bb_test")};
	EXPECT_FALSE(ch->isSet(data));
	BoxButton btn(data);
	EXPECT_TRUE(ch->isSet(data));
}

// Destructor unregisters the Data from its collection.
TEST_F(BoxButtonTest, DestructorUnregisters) {
	StringUMap d{{"name", "Item"}};
	auto data{new TestData(d)};
	auto ch{CollectionHandler::getInstance("bb_test")};
	{
		BoxButton btn(data);
		EXPECT_TRUE(ch->isSet(data));
	}
	EXPECT_FALSE(ch->isIdSet("Item"));
}

// getData() returns the owned pointer.
TEST_F(BoxButtonTest, GetDataReturnsPointer) {
	StringUMap d{{"name", "MyItem"}};
	auto data{new TestData(d)};
	BoxButton btn(data);
	EXPECT_EQ(data, btn.getData());
}

// sync() reflects createPrettyName().
TEST_F(BoxButtonTest, UpdateLabelReflectsPrettyName) {
	StringUMap d{{"name", "TestLabel"}};
	auto data{new TestData(d)};
	BoxButton btn(data);
	btn.sync();
	EXPECT_EQ("TestLabel", btn.getLabel()->get_text());
}

// Move constructor nulls the source data pointer.
TEST_F(BoxButtonTest, MoveConstructorNullsSource) {
	StringUMap d{{"name", "Mover"}};
	auto data{new TestData(d)};
	BoxButton src(data);
	BoxButton dst(std::move(src));
	EXPECT_EQ(nullptr, src.getData());
	EXPECT_EQ(data,    dst.getData());
}

// Destroying a moved-from BoxButton does not crash or double-unregister.
TEST_F(BoxButtonTest, MovedFromDestructionSafe) {
	StringUMap d{{"name", "Safe"}};
	auto data{new TestData(d)};
	auto ch{CollectionHandler::getInstance("bb_test")};
	auto src{new BoxButton(data)};
	BoxButton  dst(std::move(*src));
	EXPECT_NO_FATAL_FAILURE(delete src);
	// dst still owns the data and it is still registered.
	EXPECT_TRUE(ch->isSet(data));
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
