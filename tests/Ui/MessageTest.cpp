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
		// Single unified message dialog.
		builder->get_widget("DialogMessage",       testDialog);
		builder->get_widget("TextViewMessageBody", testBody);
		Message::initialize(builder, mainWindow.get());
		testDialog->signal_show().connect([&] () {
			testDialog->close();
		});
	}

	void TearDown() override {
		mainWindow.reset();
		builder.reset();
		app.reset();
	}

	std::string bodyText() const {
		return testBody->get_buffer()->get_text();
	}

	Glib::RefPtr<Gtk::Application> app;
	Glib::RefPtr<Gtk::Builder>     builder;
	std::unique_ptr<Gtk::Window>   mainWindow;
	Gtk::Dialog*   testDialog = nullptr;
	Gtk::TextView* testBody   = nullptr;
};

// Test case: ConstructorSimple
TEST_F(MessageTest, ConstructorSimple) {
	Message msg("Test error");
	EXPECT_EQ(msg.takeMessage(), "Test error");
}

// Test case: DisplayErrorInstance
TEST_F(MessageTest, DisplayErrorInstance) {
	Message msg("Instance error");
	msg.displayError(mainWindow.get());
	EXPECT_EQ(bodyText(), "Instance error");
}

// Test case: DisplayErrorStatic
TEST_F(MessageTest, DisplayErrorStatic) {
	Message::displayError("Static error");
	EXPECT_EQ(bodyText(), "Static error");
}

// Test case: DisplayInfo
TEST_F(MessageTest, DisplayInfo) {
	Message::displayInfo("Info message", mainWindow.get());
	EXPECT_EQ(bodyText(), "Info message");
}

// Test case: AskYes
TEST_F(MessageTest, AskYes) {
	const auto result = Message::ask("Do you agree?", mainWindow.get());
	// due to close() without answer but is expected.
	EXPECT_EQ(result, Gtk::ResponseType::RESPONSE_DELETE_EVENT);
	EXPECT_EQ(bodyText(), "Do you agree?");
}

// Test case: GetMessage
TEST_F(MessageTest, GetMessage) {
	Message msg("Temp error");
	EXPECT_EQ(msg.takeMessage(), "Temp error");
	EXPECT_EQ(msg.takeMessage(), "");
}

// Test case: BatchStartsInactive
TEST_F(MessageTest, BatchStartsInactive) {
	// Ensure no prior test left batching on.
	Message::endBatch();
	EXPECT_FALSE(Message::isBatching());
}

// Test case: BeginBatchTurnsOnBatching
TEST_F(MessageTest, BeginBatchTurnsOnBatching) {
	Message::beginBatch();
	EXPECT_TRUE(Message::isBatching());
	Message::endBatch();
}

// Test case: CollectAccumulatesWhileBatching
TEST_F(MessageTest, CollectAccumulatesWhileBatching) {
	Message::beginBatch();
	Message::collect("first");
	Message::collect("second");
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "first\nsecond\n");
}

// Test case: CollectIgnoredWhenNotBatching
TEST_F(MessageTest, CollectIgnoredWhenNotBatching) {
	Message::endBatch();   // ensure off
	Message::collect("should be dropped");
	Message::beginBatch();
	const auto report = Message::endBatch();
	EXPECT_TRUE(report.empty());
}

// Test case: EndBatchTurnsOffBatching
TEST_F(MessageTest, EndBatchTurnsOffBatching) {
	Message::beginBatch();
	Message::collect("x");
	Message::endBatch();
	EXPECT_FALSE(Message::isBatching());
}

// Test case: EndBatchClearsBuffer
TEST_F(MessageTest, EndBatchClearsBuffer) {
	Message::beginBatch();
	Message::collect("once");
	(void)Message::endBatch();
	// Next batch must start empty.
	Message::beginBatch();
	const auto second = Message::endBatch();
	EXPECT_TRUE(second.empty());
}

// Test case: BeginBatchResetsBuffer
TEST_F(MessageTest, BeginBatchResetsBuffer) {
	Message::beginBatch();
	Message::collect("stale");
	// Restart without endBatch() — should still drop the stale line.
	Message::beginBatch();
	Message::collect("fresh");
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "fresh\n");
}

// Test case: ThrownMessageRoutedThroughCollect
TEST_F(MessageTest, ThrownMessageRoutedThroughCollect) {
	Message::beginBatch();
	try {
		throw Message("boom");
	}
	catch (Message& e) {
		Message::collect(e.takeMessage());
	}
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "boom\n");
}

// Test case: ConsecutiveDuplicatesAreCollapsed
TEST_F(MessageTest, ConsecutiveDuplicatesAreCollapsed) {
	Message::beginBatch();
	Message::collect("same");
	Message::collect("same");
	Message::collect("same");
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "same (\xc3\x97""3)\n");
}

// Test case: SingleOccurrenceHasNoCountSuffix
TEST_F(MessageTest, SingleOccurrenceHasNoCountSuffix) {
	Message::beginBatch();
	Message::collect("once");
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "once\n");
}

// Test case: NonConsecutiveDuplicatesAreCollapsedTogether
TEST_F(MessageTest, NonConsecutiveDuplicatesAreCollapsedTogether) {
	// endBatch sorts before collapsing, so duplicates interleaved with
	// other lines still merge into a single (×N) entry.
	Message::beginBatch();
	Message::collect("A");
	Message::collect("B");
	Message::collect("A");
	Message::collect("A");
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "A (\xc3\x97""3)\nB\n");
}

// Test case: ReportIsSortedAlphabetically
TEST_F(MessageTest, ReportIsSortedAlphabetically) {
	Message::beginBatch();
	Message::collect("zeta");
	Message::collect("alpha");
	Message::collect("mu");
	const auto report = Message::endBatch();
	EXPECT_EQ(report, "alpha\nmu\nzeta\n");
}
