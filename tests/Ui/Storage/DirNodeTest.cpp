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

#include "MockBasicData.hpp"
#include "Storage/DirNode.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Test::Mocks::MockBasicData;

// Minimal concrete class for testing DirNode in isolation.
class TestNode : public MockBasicData, public DirNode {

public:

	TestNode(Values& data, DirNode* parent, const string& filename) noexcept :
		MockBasicData(data),
		DirNode(getProperties(), parent, filename)
	{}
};

TEST(DirNodeTest, TestFunctionality) {

	Values data;
	TestNode root  {data, nullptr, "root"};
	TestNode child {data, &root,   "child"};
	TestNode grand {data, &child,  "deep"};

	// UID written and non-empty.
	EXPECT_FALSE(root.getProperties().getValue(UID).empty());

	// PID is empty at root level.
	EXPECT_EQ(emptyString, root.getProperties().getValue(PID));

	// PID matches parent UID.
	EXPECT_EQ(root.getProperties().getValue(UID),  child.getProperties().getValue(PID));
	EXPECT_EQ(child.getProperties().getValue(UID), grand.getProperties().getValue(PID));

	// getFsId() aliases UID.
	EXPECT_EQ(root.getProperties().getValue(UID),  root.getFsId());
	EXPECT_EQ(child.getProperties().getValue(UID), child.getFsId());

	// getName() returns the filename segment.
	EXPECT_EQ("root",  root.getName());
	EXPECT_EQ("child", child.getName());
	EXPECT_EQ("deep",  grand.getName());

	// getParent() and isAtRoot().
	EXPECT_EQ(nullptr, root.getParent());
	EXPECT_EQ(&root,   child.getParent());
	EXPECT_TRUE(root.isAtRoot());
	EXPECT_FALSE(child.isAtRoot());

	// getPath() returns the parent's full path.
	EXPECT_EQ(emptyString,  root.getPath());
	EXPECT_EQ("root",       child.getPath());
	EXPECT_EQ("root/child", grand.getPath());

	// getFullPath() includes own name segment.
	EXPECT_EQ("root",            root.getFullPath());
	EXPECT_EQ("root/child",      child.getFullPath());
	EXPECT_EQ("root/child/deep", grand.getFullPath());

}
