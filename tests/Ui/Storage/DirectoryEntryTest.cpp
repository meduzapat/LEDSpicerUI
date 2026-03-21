/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryEntryTest.cpp
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
#include "Storage/DirectoryEntry.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;

class DirectoryEntryTest : public ::testing::Test {

protected:

	void SetUp() override {
		StringUMap rootData{{NAME, "root"}};
		root = std::make_unique<DirectoryEntry>(rootData, nullptr);

		StringUMap childData{{NAME, "subdir"}};
		child = std::make_unique<DirectoryEntry>(childData, root.get());

		StringUMap grandData{{NAME, "deep"}};
		grand = std::make_unique<DirectoryEntry>(grandData, child.get());
	}

	std::unique_ptr<DirectoryEntry> root;
	std::unique_ptr<DirectoryEntry> child;
	std::unique_ptr<DirectoryEntry> grand;
};

// PID is empty at root level.
TEST_F(DirectoryEntryTest, PidEmptyAtRoot) {
	EXPECT_EQ("", root->getProperty(PID));
}

// PID matches parent UID for child.
TEST_F(DirectoryEntryTest, PidMatchesParentUid) {
	EXPECT_EQ(root->getProperty(UID),  child->getProperty(PID));
	EXPECT_EQ(child->getProperty(UID), grand->getProperty(PID));
}

// UID is set and starts with "dir_".
TEST_F(DirectoryEntryTest, UidSetAtConstruction) {
	const string uid(root->getProperty(UID));
	EXPECT_FALSE(uid.empty());
	EXPECT_EQ(0, uid.find("dir_"));
}

// getFsId() is an alias for getProperty(UID).
TEST_F(DirectoryEntryTest, GetFsIdAliasesUid) {
	EXPECT_EQ(root->getProperty(UID),  root->getFsId());
	EXPECT_EQ(child->getProperty(UID), child->getFsId());
}

// getName() returns the NAME field value.
TEST_F(DirectoryEntryTest, GetName) {
	EXPECT_EQ("root",   root->getName());
	EXPECT_EQ("subdir", child->getName());
	EXPECT_EQ("deep",   grand->getName());
}

// isAtRoot() reflects parent pointer state.
TEST_F(DirectoryEntryTest, IsRootAtRoot) {
	EXPECT_TRUE(root->isAtRoot());
}

TEST_F(DirectoryEntryTest, IsRootNotAtRoot) {
	EXPECT_FALSE(child->isAtRoot());
	EXPECT_FALSE(grand->isAtRoot());
}

// Path resolution via DirNode chain.
TEST_F(DirectoryEntryTest, GetFullPathRoot) {
	EXPECT_EQ("root", root->getFullPath());
}

TEST_F(DirectoryEntryTest, GetFullPathChild) {
	EXPECT_EQ("root/subdir", child->getFullPath());
}

TEST_F(DirectoryEntryTest, GetFullPathGrandchild) {
	EXPECT_EQ("root/subdir/deep", grand->getFullPath());
}

// createUniqueId() uses PID + NAME.
TEST_F(DirectoryEntryTest, CreateUniqueIdAtRoot) {
	const string expected(Defaults::createCommonUniqueId({"", "root"}));
	EXPECT_EQ(expected, root->createUniqueId());
}

TEST_F(DirectoryEntryTest, CreateUniqueIdNested) {
	const string expected(Defaults::createCommonUniqueId({
		root->getProperty(UID), "subdir"
	}));
	EXPECT_EQ(expected, child->createUniqueId());
}

// isEmpty() reflects whether contents has any items.
TEST_F(DirectoryEntryTest, IsEmptyWhenNoContents) {
	EXPECT_TRUE(root->isEmpty());
}

// getContents() returns the mutable collection.
TEST_F(DirectoryEntryTest, GetContentsIsWritable) {
	EXPECT_EQ(0, root->getContents().getSize());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
