/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectoryNavigator.cpp
 * @since     Feb 15, 2026
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

#include "DirectoryNavigator.hpp"

using namespace LEDSpicerUI::Ui;

DirectoryNavigator::~DirectoryNavigator() {
	delete DataDialogs::DialogDirectory::getInstance();
}

void DirectoryNavigator::onActivate() noexcept {
	wireDialogs(currentDir);
}

void DirectoryNavigator::enterDirectory(Storage::DirectoryEntry* dir) noexcept {
	currentDir = dir;
	wireDialogs(currentDir);
}

void DirectoryNavigator::navigateUp() noexcept {
	if (currentDir == &rootDir) return;
	Storage::DirNode* parent = currentDir->getParent();
	currentDir = parent ? static_cast<Storage::DirectoryEntry*>(parent) : &rootDir;
	wireDialogs(currentDir);
}

bool DirectoryNavigator::isAtRoot() const noexcept {
	return currentDir == &rootDir;
}

Storage::DirectoryEntry* DirectoryNavigator::getCurrentDir() const noexcept {
	return currentDir;
}

DirectoryNavigator::DirectoryNavigator(const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	rootDir(rootData, nullptr),
	currentDir(&rootDir)
{
	DataDialogs::DialogDirectory::buildInstance(builder, "DialogDirectory");
}

void DirectoryNavigator::loadFromDisk(const string& dir) noexcept {
	namespace fs = std::filesystem;
	std::error_code ec;
	fs::path root(dir);
	if (not fs::is_directory(root, ec)) return;

	std::function<void(const fs::path&, Storage::DirectoryEntry*)> walk {

	[&](const fs::path& path, Storage::DirectoryEntry* node) {

		vector<fs::directory_entry> entries;

		for (auto& e : fs::directory_iterator(path, ec)) entries.push_back(e);
		std::sort(entries.begin(), entries.end());

		for (auto& entry : entries) {

			if (entry.is_directory(ec)) {
				StringUMap data{{NAME, entry.path().filename().string()}};
				auto child {new Storage::DirectoryEntry(data, node)};
				node->getContents().create(child);
				walk(entry.path(), child);
			}
			else if (entry.is_regular_file(ec) and entry.path().extension() == ".xml") {
				loadFile(entry.path().string(), node);
			}
		}
	}};
	walk(root, &rootDir);
}
