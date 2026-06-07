/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LeafDataTest.cpp
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
#include "Storage/Element.hpp"
#include "Storage/RestrictorMap.hpp"
#include "Storage/InputMap.hpp"
#include "Storage/InputMapLink.hpp"
#include "Storage/Process.hpp"
#include "ElementObserverMock.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;
using LEDSpicerUI::Test::Mocks::MockElementObserver;

// Element -------------------------------------------------------------

TEST(ElementTest, TestFunctionality) {

	Values data {{NAME, "P1_BUTTON1"}, {BRIGHTNESS, DEFAULT_BRIGHTNESS}};
	Element e {data};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_ELEMENT_BOX_BUTTON, e.getCssClass());
	EXPECT_EQ(TYPE_ELEMENT,           e.getXmlTag());
	EXPECT_NE(nullptr,                e.getCollectionHandler());

	// createPrettyName plain, then with strip descriptor.
	EXPECT_EQ("P1_BUTTON1", e.createPrettyName());

	e.addStripChild(new Element);
	e.addStripChild(new Element);
	e.addStripChild(new Element);
	e.addStripChild(new Element);
	EXPECT_EQ(4, e.copyStripChildren().size());
	e.getProperties().setValue(PROP_STRIP_UID, "1");
	e.setValue(STRIPSIZE, "4");
	EXPECT_EQ("P1_BUTTON1 [4]", e.createPrettyName());

	e.deleteExcessStripChildren(2);
	EXPECT_EQ(2, e.copyStripChildren().size());

	e.clearStripChildren();
	EXPECT_EQ(0, e.copyStripChildren().size());

	// shouldSerialize: default brightness suppressed, non-default included.
	EXPECT_EQ(string::npos, e.toXML().find(BRIGHTNESS));
	e.setValue(BRIGHTNESS, "50");
	EXPECT_NE(string::npos, e.toXML().find(BRIGHTNESS));

	// findFirstConnectorIndexByPosition: 0-based pin index from 1-based position.
	EXPECT_EQ(0u, Element::findFirstConnectorIndexByPosition("1"));
	EXPECT_EQ(3u, Element::findFirstConnectorIndexByPosition("2"));
	EXPECT_EQ(6u, Element::findFirstConnectorIndexByPosition("3"));

	// convertPositionToRGB: sets RED/GREEN/BLUE pins from position and colorFormat.
	Values rgb;
	Element rgbEl {rgb};
	Element::convertPositionToRGB(&rgbEl, "1", "RGB");
	EXPECT_EQ("0", rgbEl.getValue(RED_PIN));
	EXPECT_EQ("1", rgbEl.getValue(GREEN_PIN));
	EXPECT_EQ("2", rgbEl.getValue(BLUE_PIN));

	// splitRGB: dispatches on POSITIONS, then STRIPSIZE, then POSITION.
	Values d1 {{POSITIONS, "2"}, {COLORFORMAT, "RGB"}};
	Element e1 {d1};
	Element::splitRGB(&e1);
	EXPECT_EQ("3", e1.getValue(RED_PIN));

	Values d2 {{STRIPSIZE, "4"}, {POSITION, "1"}, {COLORFORMAT, "RGB"}};
	Element e2 {d2};
	Element::splitRGB(&e2);
	EXPECT_EQ("0", e2.getValue(RED_PIN));

	Values d3 {{POSITION, "3"}, {COLORFORMAT, "RGB"}};
	Element e3 {d3};
	Element::splitRGB(&e3);
	EXPECT_EQ("6", e3.getValue(RED_PIN));

}

TEST(ElementTest, TestCalledFlags) {

	Element dummyElement;
	auto observer {static_cast<MockElementObserver*>(dummyElement.getObserver())};
	observer->reset();
	EXPECT_FALSE(observer->isOnAddedCalled());
	EXPECT_FALSE(observer->isOnRemovedCalled());
	EXPECT_FALSE(observer->isOnChangedCalled());

	// test onAdded: register element to collection, which triggers onAdded.
	dummyElement.registerToCollection();
	EXPECT_TRUE(observer->isOnAddedCalled());
	// test onRemoved: unregister element from collection, which triggers onRemoved.
	dummyElement.unregisterFromCollection();
	EXPECT_TRUE(observer->isOnRemovedCalled());
}

// RestrictorMap -------------------------------------------------------

TEST(RestrictorMapTest, TestFunctionality) {

	Values data {{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r {data};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_RESTRICTOR_MAP_BOX_BUTTON, r.getCssClass());
	EXPECT_EQ(TYPE_MAP,                      r.getXmlTag());
	EXPECT_NE(nullptr,                       r.getCollectionHandler());

	// createUniqueId combines player and joystick.
	EXPECT_EQ(Defaults::createCommonUniqueId({"1", "1"}), r.createUniqueId());

	// createPrettyName basic, then with interface.
	EXPECT_EQ("Player 1 Joystick 1", r.createPrettyName());
	r.setValue(RESTRICTOR_INTERFACE, "3");
	EXPECT_NE(string::npos, r.createPrettyName().find("interface 3"));

	// toXML emits map tag.
	const string xml(r.toXML());
	EXPECT_EQ("<map\n\tplayer=\"1\"\n\tjoystick=\"1\"\n\tid=\"3\"\n/>\n", xml);

}

// InputMap ------------------------------------------------------------

TEST(InputMapTest, TestFunctionality) {

	Values targetData {{NAME, "P1_BUTTON1"}};
	Element target {targetData};

	Values data {{TRIGGER, "305"}, {TYPE, ELEMENT}};
	InputMap im {data, &target};

	// getCssClass, getCollectionHandler.
	EXPECT_EQ(CSS_INPUT_MAP_BOX_BUTTON, im.getCssClass());
	EXPECT_NE(nullptr,                  im.getCollectionHandler());

	// createPrettyName.
	EXPECT_EQ("305 → P1_BUTTON1", im.createPrettyName());

	// createUniqueId: no PID, then with PID set.
	EXPECT_EQ(Defaults::createCommonUniqueId({"", "305"}), im.createUniqueId());
	im.getProperties().setValue(PID, "s_1");
	EXPECT_EQ(Defaults::createCommonUniqueId({"s_1", "305"}), im.createUniqueId());

	// createTooltip.
	EXPECT_EQ("Trigger: 305 will activate " + string(ELEMENT) + "P1_BUTTON1", im.createTooltip());

}

// InputMapLink --------------------------------------------------------

class InputMapLinkTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(InputMapLinkTest, TestFunctionality) {

	Values data;
	InputMapLink link {data, "owner_1"};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_LINK_BOX_BUTTON, link.getCssClass());
	EXPECT_EQ(emptyString,         link.getXmlTag());
	EXPECT_EQ(nullptr,             link.getCollectionHandler());

	// createUniqueId returns empty; createPrettyName and createTooltip with no children.
	EXPECT_TRUE(link.createUniqueId().empty());
	EXPECT_EQ("Empty",          link.createPrettyName());
	EXPECT_EQ("No maps linked.", link.createTooltip());

	// toXML with no children is empty.
	EXPECT_TRUE(link.toXML().empty());

	// createPrettyName and createTooltip with children.
	const string lKey {NAME};
	const string lType {TYPE_MAP};
	const vector<Link::LinkField> lf {};
	Values m1d {{NAME, "m1"}}, m2d {{NAME, "m2"}};
	Element map1 {m1d}, map2 {m2d};
	Values ld1, ld2;
	link.getPrimaryChild()->create(new Link(ld1, lKey, lType, lf, &map1));
	link.getPrimaryChild()->create(new Link(ld2, lKey, lType, lf, &map2));
	EXPECT_EQ("Linked 2 Maps", link.createPrettyName());
	EXPECT_NE(string::npos, link.createTooltip().find("m1"));
	EXPECT_NE(string::npos, link.createTooltip().find("m2"));

}

TEST_F(InputMapLinkTest, ToXMLOrdering) {

	// Populate collection: std::map iterates alphabetically — m1→0, m2→1, m3→2.
	Values d1 {{NAME, "m1"}}, d2 {{NAME, "m2"}}, d3 {{NAME, "m3"}};
	Element map1 {d1}, map2 {d2}, map3 {d3};
	auto tempMaps {CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)};
	tempMaps->add(&map1);
	tempMaps->add(&map2);
	tempMaps->add(&map3);
	map1.getProperties().setValue(IID, "owner_1");
	map2.getProperties().setValue(IID, "owner_1");
	map3.getProperties().setValue(IID, "owner_1");

	Values imlData;
	InputMapLink iml {imlData, "owner_1"};

	const string lKey {NAME};
	const string lType {TYPE_MAP};
	const vector<Link::LinkField> lf {};
	Values ld1, ld3;

	// Insert map3 first — output must still follow collection order, not insertion.
	iml.getPrimaryChild()->create(new Link(ld3, lKey, lType, lf, &map3));
	iml.getPrimaryChild()->create(new Link(ld1, lKey, lType, lf, &map1));

	// map2 unlinked — absent. Output follows m1(0), m3(2) order.
	EXPECT_EQ("0,2", iml.toXML());

}

// Process -------------------------------------------------------------

TEST(ProcessTest, TestFunctionality) {

	Values data {{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p {data};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_PROCESS_BOX_BUTTON, p.getCssClass());
	EXPECT_EQ(TYPE_MAP,               p.getXmlTag());
	EXPECT_NE(nullptr,                p.getCollectionHandler());

	// createPrettyName and createUniqueId.
	EXPECT_EQ("Process: mame System: Linux", p.createPrettyName());
	EXPECT_EQ("mame", p.createUniqueId());

	// toXML.
	const string pxml(p.toXML());
	EXPECT_EQ("<map processName=\"mame\" system=\"Linux\"/>\n", pxml);

}

int main(int argc, char** argv) {
	MockElementObserver* observer {new MockElementObserver()};
	Element::setObserver(observer);
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	auto result = RUN_ALL_TESTS();
	delete observer;
	return result;
}
