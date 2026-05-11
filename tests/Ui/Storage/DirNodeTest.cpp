/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirNodeTest.cpp
 * @since     Apr 10, 2026
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
#include "Storage/DirNode.hpp"
#include "Storage/Data.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;

// Minimal concrete consumer for testing DirNode in isolation.
class TestNode : public Data, public DirNode {

public:

	TestNode(
		StringUMap&   data,
		DirNode*      parent,
		const string& filename
	) noexcept :
		Data(data),
		DirNode(getProperties(), parent, filename)
	{}

	constexpr string_view getCssClass()          const noexcept override { return "test"; }
	const string& getXmlTag()            const noexcept override { return "test"; }
	CollectionHandler*    getCollectionHandler() const noexcept override { return nullptr; }

};

class DirNodeTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap d1, d2, d3;
		rootNode   = std::make_unique<TestNode>(d1, nullptr,          "myfile");
		parentNode = std::make_unique<TestNode>(d2, nullptr,          "stub_1");
		nestedNode = std::make_unique<TestNode>(d3, parentNode.get(), "nestedfile");
	}

	std::unique_ptr<TestNode> rootNode;
	std::unique_ptr<TestNode> parentNode;
	std::unique_ptr<TestNode> nestedNode;

};

// UID is set at construction and is non-empty.
TEST_F(DirNodeTest, UidSetAtConstruction) {
	EXPECT_FALSE(rootNode->getProperties().getValue(UID).empty());
}

// UID is a plain number — no prefix.
TEST_F(DirNodeTest, UidIsNumeric) {
	string uid(rootNode->getProperties().getValue(UID));
	for (const char c : uid)
		EXPECT_TRUE(std::isdigit(c));
}

// getFsId() aliases dest UID.
TEST_F(DirNodeTest, GetFsIdAliasesUid) {
	EXPECT_EQ(rootNode->getProperties().getValue(UID),   rootNode->getFsId());
	EXPECT_EQ(nestedNode->getProperties().getValue(UID), nestedNode->getFsId());
}

// PID is empty at root level.
TEST_F(DirNodeTest, PidEmptyAtRoot) {
	EXPECT_EQ(emptyString, rootNode->getProperties().getValue(PID));
}

// PID matches parent getFsId() for nested node.
TEST_F(DirNodeTest, PidMatchesParentFsId) {
	EXPECT_EQ(parentNode->getFsId(), nestedNode->getProperties().getValue(PID));
}

// FILENAME written into dest at construction.
TEST_F(DirNodeTest, FilenameInDest) {
	EXPECT_EQ("myfile",     rootNode->getProperties().getValue(FILENAME));
	EXPECT_EQ("nestedfile", nestedNode->getProperties().getValue(FILENAME));
}

// getName() returns dest FILENAME.
TEST_F(DirNodeTest, GetNameReturnsFilename) {
	EXPECT_EQ("myfile",     rootNode->getName());
	EXPECT_EQ("nestedfile", nestedNode->getName());
}

// isAtRoot() reflects parent pointer state.
TEST_F(DirNodeTest, IsAtRootWhenNoParent) {
	EXPECT_TRUE(rootNode->isAtRoot());
	EXPECT_TRUE(parentNode->isAtRoot());
}

TEST_F(DirNodeTest, IsNotAtRootWhenNested) {
	EXPECT_FALSE(nestedNode->isAtRoot());
}

// getParent() returns correct pointer.
TEST_F(DirNodeTest, GetParentReturnsParent) {
	EXPECT_EQ(nullptr,           rootNode->getParent());
	EXPECT_EQ(parentNode.get(),  nestedNode->getParent());
}

// getPath() returns parent full path, empty at root.
TEST_F(DirNodeTest, GetPathEmptyAtRoot) {
	EXPECT_EQ(emptyString, rootNode->getPath());
}

TEST_F(DirNodeTest, GetPathReturnsParentFullPath) {
	EXPECT_EQ("stub_1", nestedNode->getPath());
}

// getFullPath() includes own name segment.
TEST_F(DirNodeTest, GetFullPathAtRoot) {
	EXPECT_EQ("myfile", rootNode->getFullPath());
}

TEST_F(DirNodeTest, GetFullPathNested) {
	EXPECT_EQ("stub_1/nestedfile", nestedNode->getFullPath());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


