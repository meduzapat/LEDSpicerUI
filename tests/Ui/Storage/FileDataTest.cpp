/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileDataTest.cpp
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
#include "Storage/FileData.hpp"
#include "Storage/DirectoryEntry.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;

// Minimal concrete FileData subclass for testing.
class TestFileData : public FileData {
public:
	TestFileData(StringUMap& data, const DirNode* parent) : FileData(data, parent) {}
	constexpr string_view getCssClass() const noexcept override { return "test"; }
};

class FileDataTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Root-level file — no parent.
		StringUMap rootData{{NAME, "Credits"}, {FILENAME, "myinput"}};
		rootFile = std::make_unique<TestFileData>(rootData, nullptr);

		// Directory for nested file.
		StringUMap dirData{{NAME, "subdir"}};
		parentDir = std::make_unique<DirectoryEntry>(dirData, nullptr);

		// Nested file inside parentDir.
		StringUMap nestedData{{NAME, "Credits"}, {FILENAME, "nestedinput"}};
		nestedFile = std::make_unique<TestFileData>(nestedData, parentDir.get());
	}

	std::unique_ptr<TestFileData>    rootFile;
	std::unique_ptr<DirectoryEntry>  parentDir;
	std::unique_ptr<TestFileData>    nestedFile;
};

// FILENAME must be in properties, not fieldsData.
TEST_F(FileDataTest, FilenameInProperties) {
	EXPECT_EQ("myinput", rootFile->getProperty(FILENAME));
	EXPECT_EQ("",        rootFile->getValue(FILENAME));
}

// FILENAME must not appear in XML output.
TEST_F(FileDataTest, FilenameNotSerialized) {
	EXPECT_EQ(string::npos, rootFile->toXML().find(FILENAME));
}

// getName() returns the stored filename property.
TEST_F(FileDataTest, GetNameReturnsFilename) {
	EXPECT_EQ("myinput", rootFile->getName());
}

// PID is empty at root level.
TEST_F(FileDataTest, PidEmptyAtRoot) {
	EXPECT_EQ("", rootFile->getProperty(PID));
}

// PID matches parent UID for nested file.
TEST_F(FileDataTest, PidMatchesParentUid) {
	EXPECT_EQ(parentDir->getProperty(UID), nestedFile->getProperty(PID));
}

// UID is set and starts with "file_".
TEST_F(FileDataTest, UidSetAtConstruction) {
	const string uid(rootFile->getProperty(UID));
	EXPECT_FALSE(uid.empty());
	EXPECT_EQ(0, uid.find("file_"));
}

// getFsId() is an alias for getProperty(UID).
TEST_F(FileDataTest, GetFsIdAliasesUid) {
	EXPECT_EQ(rootFile->getProperty(UID), rootFile->getFsId());
}

// createUniqueId() == createCommonUniqueId({PID, FILENAME}).
TEST_F(FileDataTest, CreateUniqueIdAtRoot) {
	const string expected(Defaults::createCommonUniqueId({"", "myinput"}));
	EXPECT_EQ(expected, rootFile->createUniqueId());
}

TEST_F(FileDataTest, CreateUniqueIdNested) {
	const string expected(Defaults::createCommonUniqueId({
		parentDir->getProperty(UID), "nestedinput"
	}));
	EXPECT_EQ(expected, nestedFile->createUniqueId());
}

// Path resolution via DirNode chain.
TEST_F(FileDataTest, GetFullPathAtRoot) {
	EXPECT_EQ("myinput", rootFile->getFullPath());
}

TEST_F(FileDataTest, GetFullPathNested) {
	EXPECT_EQ("subdir/nestedinput", nestedFile->getFullPath());
}

/* isAtRoot() reflects parent pointer state. */
TEST_F(FileDataTest, IsRootAtRoot) {
	EXPECT_TRUE(rootFile->isAtRoot());
}

TEST_F(FileDataTest, IsRootNested) {
	EXPECT_FALSE(nestedFile->isAtRoot());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

