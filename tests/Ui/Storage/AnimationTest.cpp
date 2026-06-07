/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AnimationTest.cpp
 * @since     May 2026
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

#include "Storage/Animation.hpp"
#include "Storage/Actor.hpp"
#include "MockBasicData.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;

// Private-base trick: ensures Values is initialized before DirNode.
struct StubValues { Values v; };

class StubDirNode : private StubValues, public DirNode {

public:

	StubDirNode() noexcept : StubValues{}, DirNode(v, nullptr, "stub") {}

};

class StubGroup : public Test::Mocks::MockBasicData {

public:

	explicit StubGroup(const string& n) noexcept : MockBasicData(emptyData()) { setValue(NAME, n); }

	string createUniqueId() const noexcept override { return getPrimaryValue(); }

};

class AnimationTest : public ::testing::Test {

protected:

	void TearDown() override {
		CollectionHandler::purgeAll();
	}
};

TEST_F(AnimationTest, AnimationBasics) {

	Values rootData {{FILENAME, "myanim"}};
	Animation rootAnim {rootData, nullptr};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_ANIMATION_BOX_BUTTON, rootAnim.getCssClass());
	EXPECT_EQ(TYPE_ANIMATION,           rootAnim.getXmlTag());
	EXPECT_NE(nullptr,                  rootAnim.getCollectionHandler());

	// Single ACTORS child collection.
	EXPECT_NE(nullptr, rootAnim.getChild(COLLECTION_ACTORS));
	EXPECT_EQ(0u,      rootAnim.getChild(COLLECTION_ACTORS)->getSize());

	// pretty name / tooltip / unique id at root vs nested.
	EXPECT_EQ("myanim", rootAnim.createPrettyName());
	EXPECT_EQ("Animation with 0 actors", rootAnim.createTooltip());
	EXPECT_EQ(Defaults::createCommonUniqueId({emptyString, "myanim"}), rootAnim.createUniqueId());

	StubDirNode stubDir;
	Values nestedData {{FILENAME, "nestedanim"}};
	Animation nestedAnim {nestedData, &stubDir};
	EXPECT_EQ(
		Defaults::createCommonUniqueId({stubDir.getFsId(), "nestedanim"}),
		nestedAnim.createUniqueId()
	);
}

TEST_F(AnimationTest, ToXMLEmpty) {

	Values data {{FILENAME, "empty"}};
	Animation anim {data, nullptr};

	// Empty animation file: header + footer only, no actor body.
	EXPECT_EQ(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<!-- This is an auto-generated file by " PACKAGE_STRING ". -->\n"
		"<LEDSpicer\n\tversion=\"1.1\"\n\ttype=\"Animation\"\n>\n</LEDSpicer>\n",
		anim.toXML()
	);
}

TEST_F(AnimationTest, ToXMLWithActors) {

	Values data {{FILENAME, "twoactors"}};
	Animation anim {data, nullptr};

	StubGroup stubAll{"All"};
	Values a1 {{TYPE, "Filler"}, {ACTOR_GROUP, "All"}, {COLOR, "Blue"}, {FILTER, "Combine"}};
	Values a2 {{TYPE, "Pulse"},  {ACTOR_GROUP, "All"}, {COLOR, "Red"},  {FILTER, "Combine"}};
	anim.getChild(COLLECTION_ACTORS)->create(new Actor(a1, &stubAll, anim.getFsId()));
	anim.getChild(COLLECTION_ACTORS)->create(new Actor(a2, &stubAll, anim.getFsId()));

	const string xml(anim.toXML());
	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<!-- This is an auto-generated file by LEDSpicerUI 0.0.15. -->\n"
			"<LEDSpicer\n\tversion=\"1.1\"\n\ttype=\"Animation\"\n>\n"
			"\t<actor\n"
				"\t\ttype=\"Filler\"\n"
				"\t\tcolor=\"Blue\"\n"
				"\t\tfilter=\"Combine\"\n"
				"\t\tgroup=\"All\"\n"
			"\t/>\n"
			"\t<actor\n"
				"\t\ttype=\"Pulse\"\n"
				"\t\tcolor=\"Red\"\n"
				"\t\tfilter=\"Combine\"\n"
				"\t\tgroup=\"All\"\n"
			"\t/>\n"
			"</LEDSpicer>\n", xml);

	EXPECT_EQ("Animation with 2 actors", anim.createTooltip());
}

// Actor --------------------------------------------------------------

TEST_F(AnimationTest, ActorBasics) {

	StubGroup stubAll{"All"};
	Values data {{TYPE, "Serpentine"}, {ACTOR_GROUP, "All"}, {COLOR, "Blue"}, {FILTER, "Combine"}};
	Actor actor {data, &stubAll, "owner_1"};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_ACTOR_BOX_BUTTON, actor.getCssClass());
	EXPECT_EQ(TYPE_ACTOR,           actor.getXmlTag());
	EXPECT_NE(nullptr,              actor.getCollectionHandler());

	// PID is the parent's id, UID is auto-generated and non-empty.
	EXPECT_EQ("owner_1", actor.getProperties().getValue(PID));
	EXPECT_FALSE(actor.getProperties().getValue(UID).empty());

	// pretty name uses type + group.
	EXPECT_EQ("Serpentine (All)", actor.createPrettyName());

	// tooltip mentions both type and group.
	EXPECT_NE(string::npos, actor.createTooltip().find("Serpentine"));
	EXPECT_NE(string::npos, actor.createTooltip().find("All"));

	// uniqueId = PID + UID (non-empty, stable for this instance).
	const string uid {actor.getProperties().getValue(UID)};
	EXPECT_EQ(Defaults::createCommonUniqueId({"owner_1", uid}), actor.createUniqueId());
}

TEST_F(AnimationTest, ActorUIDIsUniquePerInstance) {

	Values d1 {{TYPE, "Filler"}}, d2 {{TYPE, "Pulse"}};
	Actor a1 {d1, nullptr, "owner_1"};
	Actor a2 {d2, nullptr, "owner_1"};
	EXPECT_NE(a1.getProperties().getValue(UID), a2.getProperties().getValue(UID));
}

TEST_F(AnimationTest, ActorScopesCollectionByParent) {

	Values d1 {{TYPE, "Filler"}}, d2 {{TYPE, "Filler"}};
	Actor a {d1, nullptr, "owner_1"};
	Actor b {d2, nullptr, "owner_2"};
	// Different parents → different scoped collections.
	EXPECT_NE(a.getCollectionHandler(), b.getCollectionHandler());
}

TEST_F(AnimationTest, ActorToXMLEmitsAttributes) {

	StubGroup stubAll{"All"};
	Values data {{TYPE, "Filler"}, {ACTOR_GROUP, "All"}, {COLOR, "Blue"}, {FILTER, "Combine"}};
	Actor actor {data, &stubAll, "owner_1"};

	const string xml(actor.toXML());
	EXPECT_EQ(
		"<actor\n"
			"\ttype=\"Filler\"\n"
			"\tcolor=\"Blue\"\n"
			"\tfilter=\"Combine\"\n"
			"\tgroup=\"All\"\n"
		"/>\n", xml);
}

TEST_F(AnimationTest, ActorEmptyValuesAreOmitted) {

	StubGroup stubAll{"All"};
	Values data {{TYPE, "Filler"}, {ACTOR_GROUP, "All"}, {COLOR, ""}};
	Actor actor {data, &stubAll, "owner_1"};

	// Empty COLOR should not appear in the serialized XML.
	const string xml(actor.toXML());
	EXPECT_EQ(string::npos, xml.find("color="));
}

TEST_F(AnimationTest, GroupDeleteCascadesAllActors) {

	Values data {{FILENAME, "cascade"}};
	Animation anim {data, nullptr};

	StubGroup stubAll{"All"};
	Values a1 {{TYPE, "Filler"},     {ACTOR_GROUP, "All"}};
	Values a2 {{TYPE, "Pulse"},      {ACTOR_GROUP, "All"}};
	Values a3 {{TYPE, "Serpentine"}, {ACTOR_GROUP, "All"}};
	anim.getChild(COLLECTION_ACTORS)->create(new Actor(a1, &stubAll, anim.getFsId()));
	anim.getChild(COLLECTION_ACTORS)->create(new Actor(a2, &stubAll, anim.getFsId()));
	anim.getChild(COLLECTION_ACTORS)->create(new Actor(a3, &stubAll, anim.getFsId()));
	ASSERT_EQ(3u, anim.getChild(COLLECTION_ACTORS)->getSize());

	// Simulate group deletion: register anim's actors as a dependency of COLLECTION_GROUPS,
	// then remove stubAll via its collection handler.
	CollectionHandler::getInstance(COLLECTION_GROUPS)->add(&stubAll);
	CollectionHandler::getInstance(COLLECTION_GROUPS)->registerDependency(
		anim.getChild(COLLECTION_ACTORS)
	);
	CollectionHandler::getInstance(COLLECTION_GROUPS)->remove(&stubAll);

	// All three actors must have been cascade-deleted.
	EXPECT_EQ(0u, anim.getChild(COLLECTION_ACTORS)->getSize());
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
