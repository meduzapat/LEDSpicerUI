/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CollectionHandlerTest.cpp
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
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Test::Mocks::MockData;
using LEDSpicerUI::Test::Mocks::collection;

class CollectionHandlerTest : public ::testing::Test {

protected:

	CollectionHandler* ch = nullptr;

	void SetUp() override {
		ch = CollectionHandler::getInstance(collection);
	}

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(CollectionHandlerTest, GetInstanceDifferentNames) {
	EXPECT_NE(CollectionHandler::getInstance(collection), CollectionHandler::getInstance("other"));
}

TEST_F(CollectionHandlerTest, ItemsRegisterstration) {
	Values d {{NAME, "A"}};
	MockData item {d};
	ch->add(&item);
	EXPECT_TRUE(ch->isSet(&item));

	ch->add(&item);
	EXPECT_EQ(1u, ch->countByKey(NAME, "A"));

	ch->remove(&item);
	EXPECT_FALSE(ch->isIdSet("A"));
}

TEST_F(CollectionHandlerTest, RemoveEmptyUidSafe) {
	MockData ghost;
	EXPECT_NO_FATAL_FAILURE(ch->remove(&ghost));
}

TEST_F(CollectionHandlerTest, GetKnownId) {
	Values d {{NAME, "A"}};
	MockData item {d};
	ch->add(&item);
	// get returns pointer for known id
	EXPECT_EQ(&item, ch->get("A"));
	// nullptr for unknown.
	EXPECT_EQ(nullptr, ch->get("nonexistent"));
}

TEST_F(CollectionHandlerTest, IsSetAndIsIdSet) {
	Values d {{NAME, "A"}};
	MockData item {d};
	ch->add(&item);
	EXPECT_TRUE(ch->isSet(&item));
	EXPECT_TRUE(ch->isIdSet("A"));
	EXPECT_FALSE(ch->isIdSet("B"));
}

// replace re-keys when ID changes.
TEST_F(CollectionHandlerTest, ReplaceReKeys) {
	Values d {{NAME, "Old"}};
	MockData item {d};
	ch->add(&item);
	item.setValue(NAME, "New");
	ch->replace(&item, "Old");
	EXPECT_FALSE(ch->isIdSet("Old"));
	EXPECT_TRUE(ch->isIdSet("New"));
}

// replace is a no-op when ID unchanged.
TEST_F(CollectionHandlerTest, ReplaceNoOpSameId) {
	Values d {{NAME, "Same"}};
	MockData item {d};
	ch->add(&item);
	ch->replace(&item, "Same");
	EXPECT_TRUE(ch->isIdSet("Same"));
}

TEST_F(CollectionHandlerTest, CountByKeyAndProperty) {
	Values d1 {{NAME, "A"}, {TYPE, "x"}};
	Values d2 {{NAME, "B"}, {TYPE, "x"}};
	Values d3 {{NAME, "C"}, {TYPE, "y"}};
	MockData i1 {d1}, i2 {d2}, i3 {d3};
	ch->add(&i1); ch->add(&i2); ch->add(&i3);

	// countByKey counts items with a matching field value.
	EXPECT_EQ(2u, ch->countByKey(TYPE, "x"));
	EXPECT_EQ(1u, ch->countByKey(TYPE, "y"));
	EXPECT_EQ(0u, ch->countByKey(TYPE, "z"));

	// findByProperty returns matching items.
	i1.getProperties().setValue(PID, "owner_1");
	i2.getProperties().setValue(PID, "owner_2");
	auto results = ch->findByProperty(PID, "owner_1");
	ASSERT_EQ(1u, results.size());
	EXPECT_EQ(&i1, results[0]);

	// hasAny returns true when at least one item matches, false when none do.
	EXPECT_TRUE(ch->hasAny(PID, "owner_1"));
	EXPECT_FALSE(ch->hasAny(PID, "owner_x"));
}

TEST_F(CollectionHandlerTest, HasAnyEmptyCollection) {
	EXPECT_FALSE(ch->hasAny(PID, "owner_1"));
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
