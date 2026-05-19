/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProjectFileTest.cpp
 * @since     Feb 9, 2026
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
#include "config/InputFile.hpp"

using namespace LEDSpicerUI::Config;
using namespace LEDSpicerUI::Constants;

/**
 * ProjectFileTest
 * Tests ProjectFile base functionality: basename extraction and parent tracking.
 * XML parsing is tested in XMLHelperTest.
 * Input-specific parsing is tested in InputFileTest.
 */
class ProjectFileTest : public ::testing::Test {};

// Basename is extracted from the full path — no extension, no directory prefix.
TEST_F(ProjectFileTest, FilenameExtractionSimple) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputSingle.xml", nullptr);
	EXPECT_EQ("inputSingle", input.getFilename());


}

TEST_F(ProjectFileTest, FilenameExtractionMulti) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputMulti.xml", nullptr);
	EXPECT_EQ("inputMulti", input.getFilename());
}

// Extension must be stripped.
TEST_F(ProjectFileTest, FilenameRemovesExtension) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputSingle.xml", nullptr);
	EXPECT_EQ(string::npos, input.getFilename().find(".xml")) << "Extension should be removed";
}

// With nullptr parent the file is at root level.
TEST_F(ProjectFileTest, NullParentReturnsNullptr) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" + PATH_INPUT + "inputSingle.xml", nullptr);
	EXPECT_EQ(nullptr, input.getParent());
}
