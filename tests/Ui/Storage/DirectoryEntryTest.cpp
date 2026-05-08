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
		StringUMap rootData{{FILENAME, "root"}};
		root = std::make_unique<DirectoryEntry>(rootData, nullptr);

		StringUMap childData{{FILENAME, "subdir"}};
		child = std::make_unique<DirectoryEntry>(childData, root.get());

		StringUMap grandData{{FILENAME, "deep"}};
		grand = std::make_unique<DirectoryEntry>(grandData, child.get());
	}

	std::unique_ptr<DirectoryEntry> root;
	std::unique_ptr<DirectoryEntry> child;
	std::unique_ptr<DirectoryEntry> grand;

};

// UID is set and is non-empty.
TEST_F(DirectoryEntryTest, UidSetAtConstruction) {
	EXPECT_FALSE(root->getProperties().getValue(UID).empty());
}

// PID is empty at root level.
TEST_F(DirectoryEntryTest, PidEmptyAtRoot) {
	EXPECT_EQ(emptyString, root->getProperties().getValue(PID));
}

// PID matches parent UID.
TEST_F(DirectoryEntryTest, PidMatchesParentUid) {
	EXPECT_EQ(root->getProperties().getValue(UID),  child->getProperties().getValue(PID));
	EXPECT_EQ(child->getProperties().getValue(UID), grand->getProperties().getValue(PID));
}

// getFsId() aliases dest UID.
TEST_F(DirectoryEntryTest, GetFsIdAliasesUid) {
	EXPECT_EQ(root->getProperties().getValue(UID),  root->getFsId());
	EXPECT_EQ(child->getProperties().getValue(UID), child->getFsId());
}

// getName() returns the name passed at construction.
TEST_F(DirectoryEntryTest, GetName) {
	EXPECT_EQ("root",   root->getName());
	EXPECT_EQ("subdir", child->getName());
	EXPECT_EQ("deep",   grand->getName());
}

// createUniqueId() uses PID + name.
TEST_F(DirectoryEntryTest, CreateUniqueIdAtRoot) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({emptyString, "root"}),
		root->createUniqueId()
	);
}

TEST_F(DirectoryEntryTest, CreateUniqueIdNested) {
	EXPECT_EQ(
		Defaults::createCommonUniqueId({root->getFsId(), "subdir"}),
		child->createUniqueId()
	);
}

// createPrettyName() prefixes with folder emoji.
TEST_F(DirectoryEntryTest, CreatePrettyName) {
	EXPECT_NE(string::npos, root->createPrettyName().find("root"));
	EXPECT_NE(string::npos, root->createPrettyName().find("📁"));
}

// createTooltip() returns full path.
TEST_F(DirectoryEntryTest, CreateTooltip) {
	EXPECT_EQ("root",        root->createTooltip());
	EXPECT_EQ("root/subdir", child->createTooltip());
}

TEST_F(DirectoryEntryTest, IsEmptyWhenNoContents) {
	EXPECT_EQ(0, root->getSize());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
