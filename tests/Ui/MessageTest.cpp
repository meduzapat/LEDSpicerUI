/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MessageTest.cpp
 * @since     Feb 20, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2025 Patricio A. Rossi (MeduZa)
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
#include <gtkmm.h>
#include "Message.hpp"
#include "Defaults.hpp"

using namespace LEDSpicerUI::Ui;

// Test fixture for Message class
class MessageTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Initialize GTK application
		int argc = 1;
		char* args[] = {const_cast<char*>("test"), nullptr};
		char** argv = args;
		app = Gtk::Application::create(argc, argv, "org.ledspicer.ui.test");

		// Create a main window for transient parent
		mainWindow = std::make_unique<Gtk::Window>();
		mainWindow->set_title("Test Window");

		try {
			builder = Gtk::Builder::create_from_file(PACKAGE_SAMPLES_DIR "data/test.ui");
		}
		catch (...) {
			FAIL() << "Failed to load: " PACKAGE_SAMPLES_DIR "data/test.ui";
		}
		// Initialize Message with test-specific dialogs
		builder->get_widget("DialogErrorMessage", testErrorDialog);
		builder->get_widget("DialogInfoMessage", testInfoDialog);
		builder->get_widget("DialogQuestionMessage", testQuestionDialog);
		Message::initialize(builder, mainWindow.get());
		testErrorDialog->signal_show().connect([&] () {
			testErrorDialog->close();
		});
		testInfoDialog->signal_show().connect([&] () {
			testInfoDialog->close();
		});
		testQuestionDialog->signal_show().connect([&] () {
			testQuestionDialog->close();
		});
	}

	void TearDown() override {
		// Restore original dialog pointers
		mainWindow.reset();
		builder.reset();
		app.reset();
	}

	Glib::RefPtr<Gtk::Application> app;
	Glib::RefPtr<Gtk::Builder> builder;
	std::unique_ptr<Gtk::Window> mainWindow;
	Gtk::MessageDialog* testErrorDialog    = nullptr;
	Gtk::MessageDialog* testInfoDialog     = nullptr;
	Gtk::MessageDialog* testQuestionDialog = nullptr;
};

// Test case: ConstructorSimple
TEST_F(MessageTest, ConstructorSimple) {
	Message msg("Test error");
	EXPECT_EQ(msg.getMessage(), "Test error");
}

// Test case: ConstructorWithDisplay
TEST_F(MessageTest, ConstructorWithDisplay) {
	EXPECT_NO_THROW({
		Message msg("Displayed error", mainWindow.get());
		EXPECT_EQ(msg.getMessage(), "Displayed error");
		EXPECT_EQ(testErrorDialog->property_secondary_text().get_value(), "Displayed error");
	});
}

// Test case: DisplayErrorInstance
TEST_F(MessageTest, DisplayErrorInstance) {
	Message msg("Instance error");
	msg.displayError(mainWindow.get());
	EXPECT_EQ(testErrorDialog->property_secondary_text().get_value(), "Instance error");
}

// Test case: DisplayErrorStatic
TEST_F(MessageTest, DisplayErrorStatic) {
	Message::displayError("Static error");
	EXPECT_EQ(testErrorDialog->property_secondary_text().get_value(), "Static error");
}

// Test case: DisplayInfo
TEST_F(MessageTest, DisplayInfo) {
	Message::displayInfo("Info message", mainWindow.get());
	EXPECT_EQ(testInfoDialog->property_secondary_text().get_value(), "Info message");
}

// Test case: AskYes
TEST_F(MessageTest, AskYes) {
	int result = Message::ask("Do you agree?", mainWindow.get());
	// due to close() without answer but is expected.
	EXPECT_EQ(result, Gtk::ResponseType::RESPONSE_DELETE_EVENT);
	EXPECT_EQ(testQuestionDialog->property_secondary_text().get_value(), "Do you agree?");
}

// Test case: GetMessage
TEST_F(MessageTest, GetMessage) {
	Message msg("Temp error");
	EXPECT_EQ(msg.getMessage(), "Temp error");
	EXPECT_EQ(msg.getMessage(), "");
}
