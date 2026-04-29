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

#include <gtest/gtest.h>
#include "Storage/CollectionHandler.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;

// Registers into "ch_test".
class TestData : public Data {

public:

	TestData(StringUMap& d) noexcept : Data(d) {}
	constexpr string_view getCssClass() const noexcept override { return "test"; }
	constexpr string_view getXmlTag()   const noexcept override { return "test"; }
	CollectionHandler* getCollectionHandler() const noexcept override {
		return CollectionHandler::getInstance("ch_test");
	}
};

class CollectionHandlerTest : public ::testing::Test {

protected:

	CollectionHandler* ch = nullptr;

	void SetUp() override {
		ch = CollectionHandler::getInstance("ch_test");
	}

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

// getInstance — same name yields same pointer.
TEST_F(CollectionHandlerTest, GetInstanceSameName) {
	EXPECT_EQ(CollectionHandler::getInstance("ch_test"), CollectionHandler::getInstance("ch_test"));
}

// getInstance — different names yield different pointers.
TEST_F(CollectionHandlerTest, GetInstanceDifferentNames) {
	EXPECT_NE(CollectionHandler::getInstance("ch_test"), CollectionHandler::getInstance("other"));
}

// add registers an item.
TEST_F(CollectionHandlerTest, AddRegistersItem) {
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	EXPECT_TRUE(ch->isSet(&item));
}

// add is idempotent — calling twice does not duplicate.
TEST_F(CollectionHandlerTest, AddIsIdempotent) {
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	ch->add(&item);
	EXPECT_EQ(1u, ch->countByKey(NAME, "A"));
}

// remove unregisters an item.
TEST_F(CollectionHandlerTest, RemoveUnregistersItem) {
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	ch->remove(&item);
	EXPECT_FALSE(ch->isIdSet("A"));
}

// remove with empty uid is safe (no crash).
TEST_F(CollectionHandlerTest, RemoveEmptyUidSafe) {
	StringUMap d;
	TestData ghost(d);
	EXPECT_NO_FATAL_FAILURE(ch->remove(&ghost));
}

// remove with unknown uid is safe (no crash).
TEST_F(CollectionHandlerTest, RemoveUnknownUidSafe) {
	StringUMap d{{NAME, "Ghost"}};
	TestData ghost(d);
	EXPECT_NO_FATAL_FAILURE(ch->remove(&ghost));
}

// get returns pointer for known id, nullptr for unknown.
TEST_F(CollectionHandlerTest, GetKnownId) {
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	EXPECT_EQ(&item, ch->get("A"));
}

TEST_F(CollectionHandlerTest, GetUnknownIdReturnsNull) {
	EXPECT_EQ(nullptr, ch->get("nonexistent"));
}

// isSet and isIdSet.
TEST_F(CollectionHandlerTest, IsSetAndIsIdSet) {
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	EXPECT_TRUE(ch->isSet(&item));
	EXPECT_TRUE(ch->isIdSet("A"));
	EXPECT_FALSE(ch->isIdSet("B"));
}

// replace re-keys when ID changes.
TEST_F(CollectionHandlerTest, ReplaceReKeys) {
	StringUMap d{{NAME, "Old"}};
	TestData item(d);
	ch->add(&item);
	item.setValue(NAME, "New");
	ch->replace(&item, "Old");
	EXPECT_FALSE(ch->isIdSet("Old"));
	EXPECT_TRUE(ch->isIdSet("New"));
}

// replace is a no-op when ID unchanged.
TEST_F(CollectionHandlerTest, ReplaceNoOpSameId) {
	StringUMap d{{NAME, "Same"}};
	TestData item(d);
	ch->add(&item);
	ch->replace(&item, "Same");
	EXPECT_TRUE(ch->isIdSet("Same"));
}

// countByKey counts items with a matching field value.
TEST_F(CollectionHandlerTest, CountByKey) {
	StringUMap d1{{NAME, "A"}, {TYPE, "x"}};
	StringUMap d2{{NAME, "B"}, {TYPE, "x"}};
	StringUMap d3{{NAME, "C"}, {TYPE, "y"}};
	TestData i1(d1), i2(d2), i3(d3);
	ch->add(&i1); ch->add(&i2); ch->add(&i3);
	EXPECT_EQ(2u, ch->countByKey(TYPE, "x"));
	EXPECT_EQ(1u, ch->countByKey(TYPE, "y"));
	EXPECT_EQ(0u, ch->countByKey(TYPE, "z"));
}

// findByProperty returns matching items.
TEST_F(CollectionHandlerTest, FindByProperty) {
	StringUMap d1{{NAME, "A"}}, d2{{NAME, "B"}};
	TestData i1(d1), i2(d2);
	i1.getProperties().setValue(PID, "owner_1");
	i2.getProperties().setValue(PID, "owner_2");
	ch->add(&i1); ch->add(&i2);
	auto results = ch->findByProperty(PID, "owner_1");
	ASSERT_EQ(1u, results.size());
	EXPECT_EQ(&i1, results[0]);
}

// hasAny returns true when at least one item matches, false when none do.
TEST_F(CollectionHandlerTest, HasAnyMatchFound) {
	StringUMap d1{{NAME, "A"}}, d2{{NAME, "B"}};
	TestData i1(d1), i2(d2);
	i1.getProperties().setValue(PID, "owner_1");
	i2.getProperties().setValue(PID, "owner_2");
	ch->add(&i1); ch->add(&i2);
	EXPECT_TRUE(ch->hasAny(PID, "owner_1"));
	EXPECT_FALSE(ch->hasAny(PID, "owner_x"));
}

// hasAny on empty collection returns false.
TEST_F(CollectionHandlerTest, HasAnyEmptyCollection) {
	EXPECT_FALSE(ch->hasAny(PID, "owner_1"));
}

// cascade — remove from handler cascades to dependent collection.
// release() MUST be called before dependent is destroyed. In production,
// Parent::~Parent() always releases before child collections are destroyed,
// preventing re-entrant deletion via BoxButton::~BoxButton.
TEST_F(CollectionHandlerTest, CascadeRemovesFromDependentCollection) {
	BoxButtonCollection dependent;

	StringUMap dm{{NAME, "Elem"}};
	auto data{new TestData(dm)};
	ch->add(data);
	dependent.create(data);     // BoxButton takes ownership; ctor re-adds (idempotent)
	ch->registerDependency({&dependent});

	EXPECT_EQ(1u, dependent.getSize());
	ch->remove(data);           // cascades: BoxButton (and data) deleted from dependent
	EXPECT_EQ(0u, dependent.getSize());
	EXPECT_FALSE(ch->isIdSet("Elem"));

	ch->release(&dependent);    // must come before dependent goes out of scope
}

// purgeAll destroys all instances — the new instance starts empty.
TEST_F(CollectionHandlerTest, PurgeAllCreatesEmptyInstance) {
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	EXPECT_TRUE(ch->isIdSet("A"));

	CollectionHandler::purgeAll();

	ch = CollectionHandler::getInstance("ch_test");
	EXPECT_FALSE(ch->isIdSet("A"));
}

// release(BoxButtonCollection*) removes dependency — no cascade after release.
TEST_F(CollectionHandlerTest, ReleaseDependencyStopsCascade) {
	BoxButtonCollection dependent;

	StringUMap dm{{NAME, "E"}};
	auto data{new TestData(dm)};
	ch->add(data);
	dependent.create(data);
	ch->registerDependency({&dependent});
	ch->release(&dependent);

	EXPECT_EQ(1u, dependent.getSize());

	// Wipe before scope ends: BoxButton dtor unregisters data from ch safely
	// since the dependency is already released (no re-entrant cascade).
	dependent.wipe();
}

// registerSensitivity — widget insensitive when collection empty.
TEST_F(CollectionHandlerTest, SensitivityInsensitiveWhenEmpty) {
	auto widget{Gtk::manage(new Gtk::Button())};
	ch->registerSensitivity(widget);
	EXPECT_FALSE(widget->is_sensitive());
}

// registerSensitivity — widget becomes sensitive after add.
TEST_F(CollectionHandlerTest, SensitivitySensitiveAfterAdd) {
	auto widget{Gtk::manage(new Gtk::Button())};
	ch->registerSensitivity(widget);
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	EXPECT_TRUE(widget->is_sensitive());
}

// remove drops sensitivity back when collection empties.
TEST_F(CollectionHandlerTest, SensitivityInsensitiveAfterRemove) {
	auto widget{Gtk::manage(new Gtk::Button())};
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	ch->registerSensitivity(widget);
	ch->remove(&item);
	EXPECT_FALSE(widget->is_sensitive());
}

// minCount threshold — sensitive only when size >= minCount.
TEST_F(CollectionHandlerTest, SensitivityRespectsMinCount) {
	auto widget{Gtk::manage(new Gtk::Button())};
	ch->registerSensitivity(widget, 2);
	StringUMap d1{{NAME, "A"}}, d2{{NAME, "B"}};
	TestData i1(d1), i2(d2);
	ch->add(&i1);
	EXPECT_FALSE(widget->is_sensitive()); // 1 < 2
	ch->add(&i2);
	EXPECT_TRUE(widget->is_sensitive());  // 2 >= 2
}

// releaseSensitive — widget no longer tracked after release.
TEST_F(CollectionHandlerTest, ReleaseSensitiveStopsTracking) {
	auto widget{Gtk::manage(new Gtk::Button())};
	ch->registerSensitivity(widget);
	ch->releaseSensitive(widget);
	StringUMap d{{NAME, "A"}};
	TestData item(d);
	ch->add(&item);
	EXPECT_FALSE(widget->is_sensitive()); // still insensitive — no longer tracked
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
