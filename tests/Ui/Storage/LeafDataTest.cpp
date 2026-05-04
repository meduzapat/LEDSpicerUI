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

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;

// Element -------------------------------------------------------------

TEST(ElementTest, CssClass) {
	StringUMap data{{NAME, "P1_BUTTON1"}};
	Element e(data);
	EXPECT_EQ("ElementBoxButton", e.getCssClass());
}

TEST(ElementTest, XmlTag) {
	StringUMap data{{NAME, "P1_BUTTON1"}};
	Element e(data);
	EXPECT_EQ("element", e.getXmlTag());
}

TEST(ElementTest, CreatePrettyNamePlain) {
	StringUMap data{{NAME, "P1_BUTTON1"}};
	Element e(data);
	EXPECT_EQ("P1_BUTTON1", e.createPrettyName());
}

TEST(ElementTest, CreatePrettyNameWithStripDescriptor) {
	StringUMap data{{NAME, "LED_STRIP"}, {STRIPSIZE, "12"}};
	Element e(data);
	e.getProperties().setValue(PROP_STRIP_UID, "1");
	EXPECT_NE(string::npos, e.createPrettyName().find("[12]"));
}

TEST(ElementTest, BrightnessDefaultNotSerialized) {
	StringUMap data{{NAME, "P1_BUTTON1"}, {BRIGHTNESS, DEFAULT_BRIGHTNESS}};
	Element e(data);
	EXPECT_EQ(string::npos, e.toXML().find(BRIGHTNESS));
}

TEST(ElementTest, BrightnessNonDefaultSerialized) {
	StringUMap data{{NAME, "P1_BUTTON1"}, {BRIGHTNESS, "50"}};
	Element e(data);
	EXPECT_NE(string::npos, e.toXML().find(BRIGHTNESS));
}

// RestrictorMap -------------------------------------------------------

TEST(RestrictorMapTest, CreateUniqueId) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	EXPECT_EQ(Defaults::createCommonUniqueId({"1", "1"}), r.createUniqueId());
}

TEST(RestrictorMapTest, CreatePrettyNameBasic) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "2"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	EXPECT_EQ("Player 1 Joystick 2", r.createPrettyName());
}

TEST(RestrictorMapTest, CreatePrettyNameWithInterface) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, "3"}};
	RestrictorMap r(data);
	EXPECT_NE(string::npos, r.createPrettyName().find("interface 3"));
}

TEST(RestrictorMapTest, ToXMLSelfClosing) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	string xml(r.toXML());
	EXPECT_NE(string::npos, xml.find("<map"));
	EXPECT_NE(string::npos, xml.find("/>"));
}

TEST(RestrictorMapTest, CssClass) {
	StringUMap data{{PLAYER, "1"}, {JOYSTICK, "1"}, {RESTRICTOR_INTERFACE, ""}};
	RestrictorMap r(data);
	EXPECT_EQ("RestrictorMapBoxButton", r.getCssClass());
}

// InputMap ------------------------------------------------------------

TEST(InputMapTest, CssClass) {
	StringUMap targetData{{NAME, "P1_BUTTON1"}};
	Element    target(targetData);
	StringUMap data{{TRIGGER, "305"}, {TYPE, ELEMENT}};
	InputMap   im(data, &target);
	EXPECT_EQ("InputMapBoxButton", im.getCssClass());
}

TEST(InputMapTest, CreatePrettyName) {
	StringUMap targetData{{NAME, "P1_BUTTON1"}};
	Element    target(targetData);
	StringUMap data{{TRIGGER, "305"}, {TYPE, ELEMENT}};
	InputMap   im(data, &target);
	string     pretty(im.createPrettyName());
	EXPECT_EQ("305 → P1_BUTTON1", pretty);
}

TEST(InputMapTest, CreateUniqueIdUsesPidAndTrigger) {
	StringUMap targetData{{NAME, "P1_BUTTON1"}};
	Element    target(targetData);
	StringUMap data{{TRIGGER, "305"}, {TYPE, ELEMENT}};
	InputMap   im(data, &target);
	im.getProperties().setValue(PID, "s_1");
	EXPECT_EQ(Defaults::createCommonUniqueId({"s_1", "305"}), im.createUniqueId());
}

TEST(InputMapTest, CreateUniqueIdEmptyPidWithoutProperty) {
	StringUMap targetData{{NAME, "P1_BUTTON1"}};
	Element    target(targetData);
	StringUMap data{{TRIGGER, "305"}, {TYPE, ELEMENT}};
	InputMap   im(data, &target);
	EXPECT_EQ(Defaults::createCommonUniqueId({"", "305"}), im.createUniqueId());
}

// InputMapLink --------------------------------------------------------

class InputMapLinkTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(InputMapLinkTest, CssClass) {
	StringUMap data;
	InputMapLink link(data, "owner_1");
	EXPECT_EQ("LinkBoxButton", link.getCssClass());
}

TEST_F(InputMapLinkTest, CreateUniqueIdReturnsEmpty) {
	StringUMap data;
	InputMapLink link(data, "owner_1");
	EXPECT_TRUE(link.createUniqueId().empty());
}

TEST_F(InputMapLinkTest, CreatePrettyNameNoChildren) {
	StringUMap data;
	InputMapLink link(data, "owner_1");
	EXPECT_EQ("Empty", link.createPrettyName());
}

TEST_F(InputMapLinkTest, ToXMLNoChildren) {
	StringUMap data;
	InputMapLink link(data, "owner_1");
	EXPECT_TRUE(link.toXML().empty());
}

TEST_F(InputMapLinkTest, ToXMLWithSingleLink) {

	StringUMap d{{NAME, "m1"}};
	Element map(d);
	map.getProperties().setValue(PID, "owner_1");
	CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)->add(&map);

	StringUMap imlData;
	InputMapLink iml(imlData, "owner_1");

	const string lKey{NAME};
	const string lType{TYPE_MAP};
	const vector<Link::LinkField> lf{};
	StringUMap ld;
	iml.getPrimaryChild()->create(new Link(ld, lKey, lType, lf, &map));

	EXPECT_EQ("0", iml.toXML());
}

TEST_F(InputMapLinkTest, ToXMLSkipsUnlinkedAndFollowsCollectionOrder) {
	// std::map iterates alphabetically by key (createUniqueId = NAME).
	// m1→idx 0, m2→idx 1, m3→idx 2.
	StringUMap d1{{NAME, "m1"}}, d2{{NAME, "m2"}}, d3{{NAME, "m3"}};
	Element    map1(d1), map2(d2), map3(d3);
	auto tempMaps {CollectionHandler::getInstance(COLLECTION_INPUT_MAPS)};
	tempMaps->add(&map1);
	tempMaps->add(&map2);
	tempMaps->add(&map3);

	map1.getProperties().setValue(PID, "owner_1");
	map2.getProperties().setValue(PID, "owner_1");
	map3.getProperties().setValue(PID, "owner_1");

	StringUMap imlData;
	InputMapLink iml(imlData, "owner_1");

	const string lKey{NAME};
	const string lType{TYPE_MAP};
	const vector<Link::LinkField> lf{};
	StringUMap ld1, ld3;

	// Insert map3 first — output must still follow collection order, not insertion.
	iml.getPrimaryChild()->create(new Link(ld3, lKey, lType, lf, &map3));
	iml.getPrimaryChild()->create(new Link(ld1, lKey, lType, lf, &map1));

	// map2 is unlinked — must be absent. Output follows m1(0), m3(2) order.
	EXPECT_EQ("0,2", iml.toXML());
}

// Process -------------------------------------------------------------

TEST(ProcessTest, CreatePrettyName) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_NE(string::npos, p.createPrettyName().find("mame"));
	EXPECT_NE(string::npos, p.createPrettyName().find("Linux"));
}

TEST(ProcessTest, CreateUniqueIdFromPrimaryKey) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_EQ("mame", p.createUniqueId());
}

TEST(ProcessTest, ToXMLContainsMapTag) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_NE(string::npos, p.toXML().find("<map"));
}

TEST(ProcessTest, CssClass) {
	StringUMap data{{PARAM_PROCESS_NAME, "mame"}, {PARAM_SYSTEM, "Linux"}};
	Process p(data);
	EXPECT_EQ("ProcessBoxButton", p.getCssClass());
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
