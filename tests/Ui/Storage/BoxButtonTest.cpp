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

TEST_F(BoxButtonTest, TestFunctionality) {

	Values d {{"name", "Item"}};
	auto data {new MockData {d}};
	auto ch {data->getCollectionHandler()};
	EXPECT_FALSE(ch->isSet(data));

	// Constructor registers; getData; sync reflects createPrettyName.
	BoxButton btn {data};
	EXPECT_TRUE(ch->isSet(data));
	EXPECT_EQ(data, btn.getData());
	EXPECT_EQ("Item", btn.getLabel()->get_text());
	data->setValue("name", "Item2");
	btn.sync();
	EXPECT_EQ("Item2", btn.getLabel()->get_text());
	ch->replace(data, "Item");

	// getData() const overload.
	const BoxButton& cbtn {btn};
	EXPECT_EQ(data, cbtn.getData());

	// operator==: identity — same object true, different object false.
	Values d2 {{"name", "Other"}};
	auto data2 {new MockData {d2}};
	BoxButton btn2 {data2};
	EXPECT_TRUE(btn  == btn);
	EXPECT_FALSE(btn == btn2);

	// Destructor unregisters both items.

}

TEST_F(BoxButtonTest, MoveLifecycle) {

	// Move constructor nulls the source data pointer.
	Values d {{"name", "Item"}};
	auto data {new MockData {d}};
	BoxButton src {data};
	BoxButton dst {std::move(src)};
	EXPECT_EQ(nullptr, src.getData());
	EXPECT_EQ(data,    dst.getData());

	// Destroying a moved-from BoxButton is safe and does not double-unregister.
	Values d2 {{"name", "Item2"}};
	auto data2 {new MockData {d2}};
	auto heapSrc {new BoxButton {data2}};
	BoxButton heapDst {std::move(*heapSrc)};
	EXPECT_NO_FATAL_FAILURE(delete heapSrc);
	EXPECT_TRUE(data2->getCollectionHandler()->isSet(data2));

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
