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

#include <gtest/gtest.h>
#include "Storage/BoxButtonCollection.hpp"
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;

class TestData : public Data {

public:

	TestData(StringUMap& d) noexcept : Data(d) {}
	constexpr string_view getCssClass() const noexcept override { return "test"; }
	constexpr string_view getXmlTag()   const noexcept override { return "test"; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
};

class BoxButtonCollectionTest : public ::testing::Test {

protected:

	BoxButtonCollection collection;

	TestData* makeData(const string& name) {
		auto d{new StringUMap{{NAME, name}}};
		owned.push_back(d);
		return new TestData(*d);
	}

	void TearDown() override {
		for (auto d : owned) delete d;
		owned.clear();
	}

	vector<StringUMap*> owned;
};

// Empty collection reports zero size.
TEST_F(BoxButtonCollectionTest, EmptySize) {
	EXPECT_EQ(0u, collection.getSize());
}

// create() increases size by one and returns a valid reference.
TEST_F(BoxButtonCollectionTest, CreateIncreasesSize) {
	collection.create(makeData("A"));
	EXPECT_EQ(1u, collection.getSize());
}

// Multiple creates accumulate.
TEST_F(BoxButtonCollectionTest, MultipleCreates) {
	collection.create(makeData("A"));
	collection.create(makeData("B"));
	collection.create(makeData("C"));
	EXPECT_EQ(3u, collection.getSize());
}

// isIdSet finds by unique ID string.
TEST_F(BoxButtonCollectionTest, IsIdSetFindsExisting) {
	collection.create(makeData("A"));
	EXPECT_TRUE(collection.isIdSet("A"));
}

TEST_F(BoxButtonCollectionTest, IsIdSetMissesAbsent) {
	collection.create(makeData("A"));
	EXPECT_FALSE(collection.isIdSet("B"));
}

// isSet finds by Data pointer equality (createUniqueId comparison).
TEST_F(BoxButtonCollectionTest, IsSetFindsExisting) {
	auto data{makeData("A")};
	collection.create(data);
	EXPECT_TRUE(collection.isSet(data));
}

TEST_F(BoxButtonCollectionTest, IsSetMissesAbsent) {
	collection.create(makeData("A"));
	StringUMap d{{NAME, "B"}};
	TestData other(d);
	EXPECT_FALSE(collection.isSet(&other));
}

// remove(BoxButton&) reduces size and the item is gone.
TEST_F(BoxButtonCollectionTest, RemoveByBoxButton) {
	BoxButton& bb = collection.create(makeData("A"));
	collection.create(makeData("B"));
	collection.remove(bb);
	EXPECT_EQ(1u, collection.getSize());
	EXPECT_FALSE(collection.isIdSet("A"));
}

// remove(Data*) finds and removes by equality.
TEST_F(BoxButtonCollectionTest, RemoveByData) {
	auto data{makeData("A")};
	collection.create(data);
	collection.create(makeData("B"));
	collection.remove(data);
	EXPECT_EQ(1u, collection.getSize());
	EXPECT_FALSE(collection.isIdSet("A"));
}

// wipe() empties the collection.
TEST_F(BoxButtonCollectionTest, WipeClearsAll) {
	collection.create(makeData("A"));
	collection.create(makeData("B"));
	collection.wipe();
	EXPECT_EQ(0u, collection.getSize());
}

// swap() exchanges contents between two collections.
TEST_F(BoxButtonCollectionTest, SwapExchangesContents) {
	BoxButtonCollection other;
	collection.create(makeData("A"));
	other.create(makeData("B"));
	collection.swap(other);
	EXPECT_TRUE(collection.isIdSet("B"));
	EXPECT_TRUE(other.isIdSet("A"));
	EXPECT_EQ(1u, collection.getSize());
	EXPECT_EQ(1u, other.getSize());
}

// Iterators cover all items.
TEST_F(BoxButtonCollectionTest, IterationCoversAll) {
	collection.create(makeData("A"));
	collection.create(makeData("B"));
	collection.create(makeData("C"));
	size_t count = 0;
	for (auto& bb : collection) {
		static_cast<void>(bb);
		++count;
	}
	EXPECT_EQ(3u, count);
}

// wipe() drops sensitivity when collection empties.
TEST_F(BoxButtonCollectionTest, WipeDropsSensitivity) {
	auto widget{Gtk::manage(new Gtk::Button())};
	collection.create(makeData("A"));
	collection.registerSensitivity(widget);
	EXPECT_TRUE(widget->is_sensitive());  // 1 >= 1
	collection.wipe();
	EXPECT_FALSE(widget->is_sensitive()); // 0 < 1
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
