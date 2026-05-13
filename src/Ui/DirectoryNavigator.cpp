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

void DirectoryNavigator::save(const string& projectDir) noexcept {

	namespace fs = std::filesystem;
	const string baseDir(projectDir + string(getSubDir()));

	std::function<void(Storage::BoxButtonCollection*)> saveDir =
	[&](Storage::BoxButtonCollection* col) {
		for (auto btn : *col) {
			auto data {btn->getData()};
			auto node {dynamic_cast<Storage::DirNode*>(data)};
			if (data->getXmlTag().empty()) {
				fs::create_directories(baseDir + node->getFullPath());
				saveDir(static_cast<Storage::DirectoryEntry*>(node)->getPrimaryChild());
			}
			else {
				const string parentPath(node->getPath());
				if (not parentPath.empty())
					fs::create_directories(baseDir + parentPath);
				saveItem(data, baseDir + node->getFullPath() + ".xml");
			}
		}
	};

	saveDir(rootDir.getPrimaryChild());
}

DirectoryNavigator::DirectoryNavigator(const Glib::RefPtr<Gtk::Builder>& builder) noexcept :
	rootDir(rootData, nullptr),
	currentDir(&rootDir)
{
	DataDialogs::DialogDirectory::buildInstance(builder, "DialogDirectory");
}

void DirectoryNavigator::process(const string& absPath, const string& relPath) noexcept {
	namespace fs = std::filesystem;
	std::error_code ec;

	vector<fs::directory_entry> entries;
	for (auto& e : fs::directory_iterator(absPath, ec)) entries.push_back(e);
	std::sort(entries.begin(), entries.end());

	for (auto& entry : entries) {
		if (entry.is_directory(ec)) {

			const string
				name {entry.path().filename().string()},
				childRel(relPath.empty() ? name : relPath + "/" + name);

			scanData[COLLECTION_DIRECTORIES].push_back({{FILENAME, name}, {PATH_PARENT, relPath}});
			process(name, childRel);
		}
		else if (entry.is_regular_file(ec) and entry.path().extension() == ".xml") {
			try {
				DataMap& fileData {extractData(entry.path().string(), relPath)};
				for (auto& [key, vec] : fileData)
					for (auto& item : vec)
						scanData[key].push_back(std::move(item));
			}
			catch (Message& e) {
				Message::displayError(
					"Skipping " + Glib::path_get_basename(entry.path().string()) + ":\n" +
					XMLHelper::cleanError(e.getMessage())
				);
			}
		}
	}
}
