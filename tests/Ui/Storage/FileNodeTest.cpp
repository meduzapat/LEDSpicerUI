/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileNodeTest.cpp
 * @since     Mar 2026
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
#include "Storage/FileNode.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;

class StubDirNode : public DirNode {
public:
	StubDirNode() : DirNode(nullptr) {}
	const string& getName()  const noexcept override { return id; }
	const string& getFsId()  const noexcept override { return id; }
private:
	inline static const string id = "stub_1";
};

class TestFileNode : public FileNode {
public:
	TestFileNode(StringUMap& data, DirNode* parent) :
		FileNode(data, parent, "test", {}) {}
	constexpr string_view getCssClass() const noexcept override { return "test"; }
};

class FileNodeTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap rootData{{NAME, "Credits"}, {FILENAME, "myinput"}};
		rootFile = std::make_unique<TestFileNode>(rootData, nullptr);

		parentDir = std::make_unique<StubDirNode>();

		StringUMap nestedData{{NAME, "Credits"}, {FILENAME, "nestedinput"}};
		nestedFile = std::make_unique<TestFileNode>(nestedData, parentDir.get());
	}

	std::unique_ptr<TestFileNode> rootFile;
	std::unique_ptr<StubDirNode>  parentDir;
	std::unique_ptr<TestFileNode> nestedFile;
};

// FILENAME must be in properties, not fieldsData.
TEST_F(FileNodeTest, FilenameInProperties) {
	EXPECT_EQ("myinput", rootFile->getProperty(FILENAME));
	EXPECT_EQ("",        rootFile->getValue(FILENAME));
}

// FILENAME must not appear in XML output.
TEST_F(FileNodeTest, FilenameNotSerialized) {
	EXPECT_EQ(string::npos, rootFile->toXML().find(FILENAME));
}

// getName() returns the stored filename property.
TEST_F(FileNodeTest, GetNameReturnsFilename) {
	EXPECT_EQ("myinput", rootFile->getName());
}

// PID is empty at root level.
TEST_F(FileNodeTest, PidEmptyAtRoot) {
	EXPECT_EQ("", rootFile->getProperty(PID));
}

// PID matches parent getFsId() for nested file.
TEST_F(FileNodeTest, PidMatchesParentFsId) {
	EXPECT_EQ(parentDir->getFsId(), nestedFile->getProperty(PID));
}

// UID is set and starts with "file_".
TEST_F(FileNodeTest, UidSetAtConstruction) {
	const string uid(rootFile->getProperty(UID));
	EXPECT_FALSE(uid.empty());
	EXPECT_EQ(0, uid.find("file_"));
}

// getFsId() is an alias for getProperty(UID).
TEST_F(FileNodeTest, GetFsIdAliasesUid) {
	EXPECT_EQ(rootFile->getProperty(UID), rootFile->getFsId());
}

// createUniqueId() == createCommonUniqueId({PID, FILENAME}).
TEST_F(FileNodeTest, CreateUniqueIdAtRoot) {
	const string expected(Defaults::createCommonUniqueId({"", "myinput"}));
	EXPECT_EQ(expected, rootFile->createUniqueId());
}

TEST_F(FileNodeTest, CreateUniqueIdNested) {
	const string expected(Defaults::createCommonUniqueId({
		parentDir->getFsId(), "nestedinput"
	}));
	EXPECT_EQ(expected, nestedFile->createUniqueId());
}

// Path resolution via DirNode chain.
TEST_F(FileNodeTest, GetFullPathAtRoot) {
	EXPECT_EQ("myinput", rootFile->getFullPath());
}

TEST_F(FileNodeTest, GetFullPathNested) {
	EXPECT_EQ("stub_1/nestedinput", nestedFile->getFullPath());
}

// isAtRoot() reflects parent pointer state.
TEST_F(FileNodeTest, IsRootAtRoot) {
	EXPECT_TRUE(rootFile->isAtRoot());
}

TEST_F(FileNodeTest, IsRootNested) {
	EXPECT_FALSE(nestedFile->isAtRoot());
}
