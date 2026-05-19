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

	Values rootData  {{FILENAME, "root"}};
	DirectoryEntry root {rootData, nullptr};

	Values childData {{FILENAME, "child"}};
	DirectoryEntry child {childData, &root};

	// getCssClass, getXmlTag, getCollectionHandler.
	EXPECT_EQ(CSS_DIRECTORY_BOX_BUTTON, root.getCssClass());
	EXPECT_EQ(emptyString,              root.getXmlTag());
	EXPECT_NE(nullptr,                  root.getCollectionHandler());

	// createUniqueId() uses PID + name.
	EXPECT_EQ(Defaults::createCommonUniqueId({emptyString,    "root"}),  root.createUniqueId());
	EXPECT_EQ(Defaults::createCommonUniqueId({root.getFsId(), "child"}), child.createUniqueId());

	// createPrettyName() appends trailing slash.
	EXPECT_EQ("root/",  root.createPrettyName());
	EXPECT_EQ("child/", child.createPrettyName());

	// createTooltip() returns full path.
	EXPECT_EQ("root",       root.createTooltip());
	EXPECT_EQ("root/child", child.createTooltip());

}
