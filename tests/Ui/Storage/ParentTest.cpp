/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ParentTest.cpp
 * @since     Mar 27, 2026
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
#include "Storage/Parent.hpp"

using namespace LEDSpicerUI::Constants;
using namespace LEDSpicerUI::Ui::Storage;
using LEDSpicerUI::Values;
using LEDSpicerUI::Test::Mocks::MockBasicData;

struct TestParent : Parent {

	TestParent(Values& d, const vector<string>& ids) : Parent(d, ids) {}

	const string& getCssClass() const noexcept override { static const string s {};        return s; }
	const string& getXmlTag()   const noexcept override { static const string s {"parent"}; return s; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
	using Parent::registerDependency;
};

struct ParentTest : ::testing::Test {

	Values data;

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(ParentTest, TestFunctionality) {

	TestParent p {data, {"A", "B"}};

	// getChild: known key returns pointer, missing key returns nullptr.
	EXPECT_NE(nullptr, p.getChild("A"));
	EXPECT_EQ(nullptr, p.getChild("MISSING"));

	// const getChild overload.
	const TestParent& cp {p};
	EXPECT_NE(nullptr, cp.getChild("A"));

	// getPrimaryChild returns first child.
	EXPECT_EQ(p.getChild("A"), p.getPrimaryChild());

	// getChildren: full map size; const overload agrees.
	EXPECT_EQ(2u, p.getChildren().size());
	EXPECT_EQ(2u, cp.getChildren().size());

	// getSize: 0 initially, counts primary child items after add.
	EXPECT_EQ(0u, p.getSize());
	Values d1 {{NAME, "first"}}, d2 {{NAME, "second"}};
	p.getChild("A")->create(new MockBasicData(d1));
	p.getChild("A")->create(new MockBasicData(d2));
	EXPECT_EQ(2u, p.getSize());

	// xmlBody emits primary children in insertion order.
	const string xml(p.toXML());
	EXPECT_EQ("<parent>\n\t<testTag name=\"first\"/>\n\t<testTag name=\"second\"/>\n</parent>\n", xml);

	// registerDependency + destructor releases cleanly.
	{
		Values depData;
		TestParent depParent {depData, {"A"}};
		depParent.registerDependency(COLLECTION_ELEMENTS, "A");
	}
	EXPECT_NO_FATAL_FAILURE(CollectionHandler::purgeAll());

}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
