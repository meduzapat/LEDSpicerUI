/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProjectFile.cpp
 * @since     May 14, 2026
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

#include "ProjectFile.hpp"

using namespace LEDSpicerUI::Config;

void ProjectFile::saveFile(const string& path, const string& content) {

	// Debug files.
	if (Settings::get().shouldDebugFiles()) {
		Gtk::Dialog dialog("DEBUG: " + path);
		dialog.set_default_size(700, 500);
		dialog.set_resizable(true);
		dialog.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
		dialog.set_transient_for(Message::getMain());

		auto scrolled {Gtk::manage(new Gtk::ScrolledWindow())};
		scrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled->set_hexpand(true);
		scrolled->set_vexpand(true);

		auto textView {Gtk::manage(new Gtk::TextView())};
		textView->get_buffer()->set_text(content);
		textView->set_editable(false);
		textView->set_monospace(true);
		scrolled->add(*textView);

		dialog.get_content_area()->pack_start(*scrolled, true, true, 0);
		dialog.show_all();
		dialog.run();
		// Drain the dialog events before returning, to avoid the dialog crashing due to remaining events running.
		while (Gtk::Main::events_pending()) Gtk::Main::iteration();
		return;
	}
	// Normal save.
	Glib::file_set_contents(path, content);
}

bool ProjectFile::saveProject(std::function<void()> doSave) {

	const auto& s {Settings::get()};

	// Debug mode: dry-run only, no filesystem side-effects from the transaction.
	if (s.shouldDebugFiles()) {
		doSave();
		return false;
	}

	namespace fs = std::filesystem;

	// Remove the trailing '/' that getProjectDir() returns.
	string projectStr {s.getProjectDir()};
	if (projectStr.back() == '/') projectStr.pop_back();

	const fs::path
		projectDir {projectStr},
		backupDir  {projectStr + BACKUP_SUFFIX};

	std::error_code ec;
	const bool projectExisted {fs::exists(projectDir)};

	// Stash the existing project into backupDir.
	if (projectExisted) {
		// rename refuses to clobber, so wipe any prior backup first. If either
		fs::remove_all(backupDir, ec);
		if (ec)
			// step fails, abort before touching the project dir.
			throw Message("Could not remove previous backup: " + ec.message());
		fs::rename(projectDir, backupDir, ec);
		if (ec)
			throw Message("Could not create backup: " + ec.message());
	}

	fs::create_directories(projectDir);
	fs::create_directories(projectDir / Constants::PATH_INPUT);
	fs::create_directories(projectDir / Constants::PATH_ANIMATION);
	fs::create_directories(projectDir / Constants::PATH_PROFILE);

	try {
		doSave();
	}
	catch (...) {
		fs::remove_all(projectDir, ec);
		if (projectExisted) {
			std::error_code revertEc;
			fs::rename(backupDir, projectDir, revertEc);
			if (revertEc)
				throw Message(
					"Save failed and the backup could not be restored. "
					"Your previous data is at " + backupDir.string() +
					". Reason: " + revertEc.message()
				);
		}
		throw;
	}

	if (projectExisted and not s.shouldSaveBackup()) {
		fs::remove_all(backupDir, ec);
		if (ec)
			Ui::StatusBar::getInstance().push(
				"Could not remove old backup",
				Ui::StatusBar::Severity::Warning
			);
		return false;
	}
	return projectExisted;
}
