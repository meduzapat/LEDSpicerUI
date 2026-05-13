/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      BoxButtonCollectionTest.cpp
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

#include "MockBasicData.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI;
using namespace Ui::Storage;
using namespace Constants;
using Test::Mocks::MockBasicData;

class BoxButtonCollectionTest : public ::testing::Test {

protected:

	BoxButtonCollection collection;

	MockBasicData* makeData(const string& name) {
		auto d {new Values {{NAME, name}}};
		owned.push_back(d);
		return new MockBasicData(*d);
	}

	void TearDown() override {
		for (auto d : owned) delete d;
		owned.clear();
	}

	vector<Values*> owned;
};

TEST_F(BoxButtonCollectionTest, ItemLifecycle) {

	// Empty on construction.
	EXPECT_EQ(0u, collection.getSize());

	// create increases size; isSet and isIdSet find existing items.
	auto* dataA {makeData("A")};
	BoxButton& bbA {collection.create(dataA)};
	EXPECT_EQ(1u,   collection.getSize());
	EXPECT_TRUE(collection.isSet(dataA));
	EXPECT_TRUE(collection.isIdSet("A"));

	collection.create(makeData("B"));
	collection.create(makeData("C"));
	EXPECT_EQ(3u, collection.getSize());

	// isSet and isIdSet miss absent items.
	Values dOther {{NAME, "Z"}};
	MockBasicData other {dOther};
	EXPECT_FALSE(collection.isSet(&other));
	EXPECT_FALSE(collection.isIdSet("Z"));

	// Non-const iteration covers all items.
	size_t count {0};
	for (auto& _ : collection) ++count;
	EXPECT_EQ(3u, count);

	// Const iteration covers all items.
	count = 0;
	const BoxButtonCollection& coll {collection};
	for (const auto& _ : coll) ++count;
	EXPECT_EQ(3u, count);

	// remove by BoxButton reference.
	collection.remove(bbA);
	EXPECT_EQ(2u,    collection.getSize());
	EXPECT_FALSE(collection.isIdSet("A"));

	// remove by Data pointer.
	auto* dataB {collection.begin()[0]->getData()};
	collection.remove(dataB);
	EXPECT_EQ(1u, collection.getSize());

}

TEST_F(BoxButtonCollectionTest, SwapLifecycle) {

	BoxButtonCollection other;
	collection.create(makeData("A"));
	other.create(makeData("B"));

	collection.swap(other);

	EXPECT_TRUE(collection.isIdSet("B"));
	EXPECT_TRUE(other.isIdSet("A"));
	EXPECT_EQ(1u, collection.getSize());
	EXPECT_EQ(1u, other.getSize());

}

TEST_F(BoxButtonCollectionTest, SensitivityLifecycle) {

	auto* widget {Gtk::manage(new Gtk::Button())};
	collection.create(makeData("A"));
	collection.registerSensitivity(widget);
	EXPECT_TRUE(widget->is_sensitive());  // 1 >= minCount(1)

	collection.wipe();
	EXPECT_EQ(0u,  collection.getSize());
	EXPECT_FALSE(widget->is_sensitive()); // 0 < minCount(1)

}

TEST_F(BoxButtonCollectionTest, MoveLifecycle) {

	// Move constructor: items transfer, source becomes empty.
	BoxButtonCollection src;
	src.create(makeData("X"));
	src.create(makeData("Y"));
	BoxButtonCollection dst {std::move(src)};
	EXPECT_EQ(0u, src.getSize());
	EXPECT_EQ(2u, dst.getSize());
	EXPECT_TRUE(dst.isIdSet("X"));
	EXPECT_TRUE(dst.isIdSet("Y"));

	// Move assignment into empty collection: items transfer.
	BoxButtonCollection dst2;
	dst2 = std::move(dst);
	EXPECT_EQ(0u, dst.getSize());
	EXPECT_EQ(2u, dst2.getSize());
	EXPECT_TRUE(dst2.isIdSet("X"));

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
