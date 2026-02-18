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

namespace LEDSpicerUI {

/**
 * ProjectFileTest
 * Tests only the unique ProjectFile functionality (filename extraction).
 * XML parsing is already tested in XMLHelperTest.
 * Input-specific functionality is tested in InputFileTest.
 */
class ProjectFileTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Set projects dir for filename extraction
		Defaults::setProjectsDir(PACKAGE_SAMPLES_DIR "data/");
	}

	void TearDown() override {
		Defaults::setProjectsDir("");
	}
};

// Test filename extraction with simple filename
TEST_F(ProjectFileTest, FilenameExtractionSimple) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputSingle.xml");
	EXPECT_EQ("inputSingle", input.getPathFilename());
}

// Test filename extraction with subdirectory
TEST_F(ProjectFileTest, FilenameExtractionWithSubdir) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputMulti.xml");
	EXPECT_EQ("inputMulti", input.getPathFilename());
}

// Test filename extraction removes extension
TEST_F(ProjectFileTest, FilenameRemovesExtension) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputSingle.xml");
	const string& filename = input.getPathFilename();
	EXPECT_EQ(string::npos, filename.find(".xml")) << "Extension should be removed";
}

// Test filename extraction is relative to project directory
TEST_F(ProjectFileTest, FilenameRelativeToProjectDir) {
	InputFile input(PACKAGE_SAMPLES_DIR "data/" INPUT_PATH "inputMulti.xml");
	const string& filename = input.getPathFilename();

	// Should not contain the projects directory path
	EXPECT_EQ(string::npos, filename.find(PACKAGE_SAMPLES_DIR))
		<< "Filename should not contain absolute path";
	EXPECT_EQ(string::npos, filename.find("data/"))
		<< "Filename should not contain project directory";
}

} // namespace LEDSpicerUI

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
