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
using LEDSpicerUI::Values;

TEST(DirectoryEntryTest, TestFunctionality) {

	Values rootData {{FILENAME, "root"}};
	DirectoryEntry root {rootData, nullptr};

	// Check empty.
	EXPECT_EQ(0, root.getSize());

	Values childData {{FILENAME, "subdir"}};
	DirectoryEntry child {childData, &root};
	Values grandData {{FILENAME, "deep"}};
	DirectoryEntry grand {grandData, &child};

	// UID is set and is non-empty.
	EXPECT_FALSE(root.getProperties().getValue(UID).empty());

	// PID is empty at root level.
	EXPECT_EQ(emptyString, root.getProperties().getValue(PID));

	// PID matches parent UID.
	EXPECT_EQ(root.getProperties().getValue(UID),  child.getProperties().getValue(PID));
	EXPECT_EQ(child.getProperties().getValue(UID), grand.getProperties().getValue(PID));

	// getFsId() aliases dest UID.
	EXPECT_EQ(root.getProperties().getValue(UID),  root.getFsId());
	EXPECT_EQ(child.getProperties().getValue(UID), child.getFsId());

	// getName() returns the name passed at construction.
	EXPECT_EQ("root",   root.getName());
	EXPECT_EQ("subdir", child.getName());
	EXPECT_EQ("deep",   grand.getName());

	// createUniqueId() uses PID + name.
	EXPECT_EQ(Defaults::createCommonUniqueId({emptyString, "root"}), root.createUniqueId());

	// Create unique Id nested.
	EXPECT_EQ(Defaults::createCommonUniqueId({root.getFsId(), "subdir"}), child.createUniqueId());

	// createPrettyName() prefixes with folder emoji.
	EXPECT_NE(string::npos, root.createPrettyName().find("root"));
	EXPECT_NE(string::npos, root.createPrettyName().find("📁"));

	// createTooltip() returns full path.
	EXPECT_EQ("root", root.createTooltip());
	EXPECT_EQ("root/subdir/deep", grand.createTooltip());

}
