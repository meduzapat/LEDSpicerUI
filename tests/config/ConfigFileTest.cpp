/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConfigFileTest.cpp
 * @since     Feb 24, 2025
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
#include "config/ConfigFile.hpp"
#include "Storage/Link.hpp"
#include "ElementObserverMock.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Config;
using namespace LEDSpicerUI::Ui::Storage;
using LEDSpicerUI::Test::Mocks::MockElementObserver;

/*
Minimal Parent stand-in for a device: expandDeviceElements only needs a
primary child collection to walk, nothing hardware-specific.
*/
class TestDevice : public Parent {

public:

	TestDevice(Values& data) noexcept : Parent(data, {COLLECTION_ELEMENTS}) {}

	const string& getXmlTag()   const noexcept override { static const string s {"device"};    return s; }
	const string& getCssClass() const noexcept override { static const string s {"TestDevice"}; return s; }
	CollectionHandler* getCollectionHandler() const noexcept override { return nullptr; }
};

class ConfigFileTest : public ::testing::Test {

protected:

	void SetUp() override {
		configFile = std::make_unique<ConfigFile>(PACKAGE_SAMPLES_DIR "data/config.xml");
	}

	void TearDown() override {
		configFile.reset();
	}

	std::unique_ptr<ConfigFile> configFile;
};

TEST_F(ConfigFileTest, SettingsLifecycle) {

	const string expectedColors {"Red, Blue, Green, Yellow, White, Black"};
	const auto& settings = configFile->getRootInfo();
	EXPECT_EQ(DEFAULT,         settings.getValue("defaultProfile"));
	EXPECT_EQ("500",           settings.getValue(PARAM_MILLISECONDS));
	EXPECT_EQ(HUMAN_TRUE,      settings.getValue("craftProfile"));
	EXPECT_EQ(HUMAN_TRUE,      settings.getValue("colorsFile"));
	EXPECT_EQ("1000",          settings.getValue("groupId"));
	EXPECT_EQ("Info",          settings.getValue("logLevel"));
	EXPECT_EQ("1000",          settings.getValue("userId"));
	EXPECT_EQ("10",            settings.getValue("fps"));
	EXPECT_EQ("1.1",           settings.getValue("version"));
	EXPECT_EQ("basicColors",   settings.getValue("colors"));
	EXPECT_EQ("file,mame",     settings.getValue("dataSource"));
	EXPECT_EQ("16161",         settings.getValue("port"));
	EXPECT_EQ("Configuration", settings.getValue("type"));
	EXPECT_EQ(expectedColors,  settings.getValue("randomColors"));

}

TEST_F(ConfigFileTest, DevicesAreProcessed) {

	auto& devices = configFile->getData(COLLECTION_DEVICES);
	EXPECT_EQ(3, devices.size());

	EXPECT_EQ("UltimarcPacDrive", devices[0].getValue(NAME));
	EXPECT_EQ("1",                devices[0].getValue(ID));
	EXPECT_EQ("121",              devices[0].getValue("changePoint"));

	EXPECT_EQ("UltimarcPacDrive", devices[1].getValue(NAME));
	EXPECT_EQ("2",                devices[1].getValue(ID));
	EXPECT_EQ("64",               devices[1].getValue("changePoint"));

	EXPECT_EQ("UltimarcUltimate", devices[2].getValue(NAME));
	EXPECT_EQ("1",                devices[2].getValue(ID));

	const string
		deviceId    = Defaults::createHardwareUniqueId({{NAME, "UltimarcPacDrive"}, {ID, "1"}}),
		elementsKey = Defaults::createCommonUniqueId({deviceId, COLLECTION_ELEMENTS});

	try {
		auto& elements = configFile->getData(elementsKey);
		EXPECT_FALSE(elements.empty());
		bool foundP1Button1 = false;
		for (const auto& element : elements) {
			if (element.getValue(NAME) == "P1_BUTTON1") {
				foundP1Button1 = true;
				EXPECT_EQ("1", element.getValue("type"));
				EXPECT_EQ("2", element.getValue("led"));
				break;
			}
		}
		EXPECT_TRUE(foundP1Button1) << "P1_BUTTON1 element not found";
	}
	catch (const std::exception& e) {
		FAIL() << "Failed to access elements with key: " << elementsKey << ", error: " << e.what();
	}

}

TEST_F(ConfigFileTest, GroupsAreProcessed) {

	auto& groups = configFile->getData(COLLECTION_GROUPS);
	EXPECT_EQ(4, groups.size());

	EXPECT_EQ("All",     groups[0].getValue(NAME));
	EXPECT_EQ("CREDITS", groups[1].getValue(NAME));
	EXPECT_EQ("STARTS",  groups[2].getValue(NAME));
	EXPECT_EQ("PLAYER1", groups[3].getValue(NAME));

	string creditsGroupKey = Defaults::createCommonUniqueId({"CREDITS", COLLECTION_GROUP_LINKS});

	try {
		auto& creditElements = configFile->getData(creditsGroupKey);
		EXPECT_EQ(3, creditElements.size());

		EXPECT_EQ("P1_CREDIT", creditElements[0].getValue(NAME));
		EXPECT_EQ("P2_CREDIT", creditElements[1].getValue(NAME));
		EXPECT_EQ("P3_CREDIT", creditElements[2].getValue(NAME));
	}
	catch (const std::exception& e) {
		FAIL() << "Failed to access group elements with key: " << creditsGroupKey << ", error: " << e.what();
	}

}

TEST_F(ConfigFileTest, ProcessesAreProcessed) {

	auto& processes = configFile->getData(COLLECTION_PROCESSES);
	EXPECT_EQ(2u, processes.size());

	EXPECT_EQ(TEST_STR, processes[0].getValue(PARAM_PROCESS_NAME));
	EXPECT_EQ("arcade", processes[0].getValue(PARAM_SYSTEM));

	EXPECT_EQ("gedit",  processes[1].getValue(PARAM_PROCESS_NAME));
	EXPECT_EQ("arcade", processes[1].getValue(PARAM_SYSTEM));

}

// ConfigFile::expandDeviceElements / matchesDeviceOrder --------------

namespace {
	const string testLinkKey  = NAME;
	const string testLinkType = TYPE_ELEMENT;
	const vector<Link::LinkField> testLinkFields {};
}

class ConfigFileSaveTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(ConfigFileSaveTest, ExpandDeviceElements_PlainElementsKeepOrder) {

	Values deviceData {{NAME, "Device1"}};
	auto device {new TestDevice(deviceData)};

	Values e1Data {{NAME, "E1"}}, e2Data {{NAME, "E2"}};
	auto& e1Btn {device->getPrimaryChild()->create(new Element(e1Data))};
	auto& e2Btn {device->getPrimaryChild()->create(new Element(e2Data))};

	BoxButtonCollection devices;
	devices.create(device);

	auto order {ConfigFile::expandDeviceElements(devices)};
	ASSERT_EQ(2u, order.size());
	EXPECT_EQ(e1Btn.getData(), order[0]);
	EXPECT_EQ(e2Btn.getData(), order[1]);

}

TEST_F(ConfigFileSaveTest, ExpandDeviceElements_StripExpandsToChildrenNotParent) {

	Values deviceData {{NAME, "Device1"}};
	auto device {new TestDevice(deviceData)};

	Values stripData {{NAME, "Strip1"}};
	auto stripElement {new Element(stripData)};
	auto c1 {new Element};
	auto c2 {new Element};
	stripElement->addStripChild(c1);
	stripElement->addStripChild(c2);
	device->getPrimaryChild()->create(stripElement);

	BoxButtonCollection devices;
	devices.create(device);

	auto order {ConfigFile::expandDeviceElements(devices)};
	ASSERT_EQ(2u, order.size());
	EXPECT_EQ(c1, order[0]);
	EXPECT_EQ(c2, order[1]);

}

TEST_F(ConfigFileSaveTest, ExpandDeviceElements_MixedPlainAndStripPreservesPosition) {

	Values deviceData {{NAME, "Device1"}};
	auto device {new TestDevice(deviceData)};

	Values e1Data {{NAME, "E1"}}, stripData {{NAME, "Strip1"}}, e2Data {{NAME, "E2"}};
	auto& e1Btn {device->getPrimaryChild()->create(new Element(e1Data))};

	auto stripElement {new Element(stripData)};
	auto c1 {new Element};
	auto c2 {new Element};
	stripElement->addStripChild(c1);
	stripElement->addStripChild(c2);
	device->getPrimaryChild()->create(stripElement);

	auto& e2Btn {device->getPrimaryChild()->create(new Element(e2Data))};

	BoxButtonCollection devices;
	devices.create(device);

	auto order {ConfigFile::expandDeviceElements(devices)};
	ASSERT_EQ(4u, order.size());
	EXPECT_EQ(e1Btn.getData(), order[0]);
	EXPECT_EQ(c1,              order[1]);
	EXPECT_EQ(c2,              order[2]);
	EXPECT_EQ(e2Btn.getData(), order[3]);

}

TEST_F(ConfigFileSaveTest, MatchesDeviceOrder_TrueWhenIdentical) {

	Values e1Data {{NAME, "E1"}}, e2Data {{NAME, "E2"}};
	Element e1 {e1Data}, e2 {e2Data};
	vector<Data*> order {&e1, &e2};

	BoxButtonCollection links;
	Values l1, l2;
	links.create(new Link(l1, testLinkKey, testLinkType, testLinkFields, &e1));
	links.create(new Link(l2, testLinkKey, testLinkType, testLinkFields, &e2));

	EXPECT_TRUE(ConfigFile::matchesDeviceOrder(links, order));

}

TEST_F(ConfigFileSaveTest, MatchesDeviceOrder_FalseWhenReordered) {

	Values e1Data {{NAME, "E1"}}, e2Data {{NAME, "E2"}};
	Element e1 {e1Data}, e2 {e2Data};
	vector<Data*> order {&e1, &e2};

	BoxButtonCollection links;
	Values l1, l2;
	links.create(new Link(l1, testLinkKey, testLinkType, testLinkFields, &e2));
	links.create(new Link(l2, testLinkKey, testLinkType, testLinkFields, &e1));

	EXPECT_FALSE(ConfigFile::matchesDeviceOrder(links, order));

}

TEST_F(ConfigFileSaveTest, MatchesDeviceOrder_FalseWhenSizeDiffers) {

	Values e1Data {{NAME, "E1"}}, e2Data {{NAME, "E2"}};
	Element e1 {e1Data}, e2 {e2Data};
	vector<Data*> order {&e1, &e2};

	BoxButtonCollection links;
	Values l1;
	links.create(new Link(l1, testLinkKey, testLinkType, testLinkFields, &e1));

	EXPECT_FALSE(ConfigFile::matchesDeviceOrder(links, order));

}

TEST_F(ConfigFileSaveTest, MatchesDeviceOrder_FalseWhenStripChildrenReordered) {

	Values stripData {{NAME, "Strip1"}};
	Element strip {stripData};
	auto c1 {new Element};
	auto c2 {new Element};
	strip.addStripChild(c1);
	strip.addStripChild(c2);

	vector<Data*> order {c1, c2};

	BoxButtonCollection links;
	Values l1, l2;
	// Reordered: c2 first, then c1 — must not match the canonical order.
	links.create(new Link(l1, testLinkKey, testLinkType, testLinkFields, c2));
	links.create(new Link(l2, testLinkKey, testLinkType, testLinkFields, c1));

	EXPECT_FALSE(ConfigFile::matchesDeviceOrder(links, order));

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
