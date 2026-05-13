/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      HardwareTest.cpp
 * @since     May 13, 2026
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
#include "Storage/Hardware.hpp"

using namespace LEDSpicerUI::Ui::Storage;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Defaults;
using LEDSpicerUI::Values;

class HardwareMock : public Hardware {

public:

	using Hardware::Hardware;

	virtual ~HardwareMock() = default;

	const string& getXmlTag()   const noexcept override { return emptyString; }
	const string& getCssClass() const noexcept override { return emptyString; }
	string createUniqueId()     const noexcept override { return emptyString; }
	CollectionHandler* getCollectionHandler() const noexcept override;
};


TEST(HardwareTest, TestcreatePrettyName) {

	Values v {{NAME, "HardwareName"}, {ID, "1"}, {PORT, ""}};

	HardwareMock h {v}; //{{NAME, "HardwareName"}, {ID, "1"}, {PORT, ""}};
	// createPrettyName: human-readable device label.
	EXPECT_EQ("", h.createPrettyName());
}

//int main(int argc, char** argv) {
//	auto app = Gtk::Application::create(argc, argv, "org.test");
//	::testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
//}
