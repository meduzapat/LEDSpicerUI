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

DirectoryNavigator::DirectoryNavigator(
	Glib::RefPtr<Gtk::Builder> const& builder,
	Storage::BoxButtonCollection*     items,
	const string&                     homeButtonName,
	const string&                     breadcrumbBoxName,
	const string&                     newDirButtonName
) :
	items(items)
{
	builder->get_widget(homeButtonName,    homeButton);
	builder->get_widget(breadcrumbBoxName, breadcrumbBox);
	builder->get_widget(newDirButtonName,  newDirButton);

	homeButton->signal_clicked().connect([this]() {
		navigateHome();
	});

	newDirButton->signal_clicked().connect([this]() {
		showNewDirectoryDialog();
	});
}

void DirectoryNavigator::refresh() {
	rebuildCache();
	populateDisplay();
}

void DirectoryNavigator::rebuildCache() {
	cachedDirs.clear();
	cachedItems.clear();

	string currentPath = getCurrentPath();
	string prefix = currentPath.empty() ? "" : currentPath + "/";

	// Scan items for directories and direct items
	for (auto* bb : *items) {
		string itemPath = bb->getData()->createUniqueId();

		// Skip items not under current path
		if (not prefix.empty() and itemPath.find(prefix) != 0) {
			continue;
		}

		// Get remainder after prefix
		string remainder = prefix.empty() ? itemPath : itemPath.substr(prefix.length());

		// Check if it's a direct item or in a subdirectory
		auto slashPos = remainder.find('/');
		if (slashPos == string::npos) {
			cachedItems.push_back(bb);
		}
		else {
			cachedDirs.insert(remainder.substr(0, slashPos));
		}
	}

	// Add empty directories at current level
	for (const auto& emptyDir : emptyDirs) {
		// Check if this empty dir is a direct child of current path
		if (prefix.empty()) {
			// At root - emptyDir must not contain '/'
			if (emptyDir.find('/') == string::npos) {
				cachedDirs.insert(emptyDir);
			}
		}
		else {
			// Not at root - emptyDir must start with prefix and have no more '/'
			if (emptyDir.find(prefix) == 0) {
				string remainder = emptyDir.substr(prefix.length());
				if (remainder.find('/') == string::npos) {
					cachedDirs.insert(remainder);
				}
			}
		}
	}

	// Cleanup: remove empty dirs that now have items
	for (auto it = emptyDirs.begin(); it != emptyDirs.end(); ) {
		bool hasItems = false;
		string dirPrefix = *it + "/";
		for (auto* bb : *items) {
			if (bb->getData()->createUniqueId().find(dirPrefix) == 0) {
				hasItems = true;
				break;
			}
		}
		if (hasItems) {
			it = emptyDirs.erase(it);
		}
		else {
			++it;
		}
	}
}

void DirectoryNavigator::populateDisplay() {
	displayBox->wipe();
	directoryEntries.wipe();

	homeButton->set_sensitive(not isAtRoot());
	updateBreadcrumb();

	// Add directories first (sorted via set)
	for (const auto& dir : cachedDirs) {
		displayBox->add(createDirectoryButton(dir));
	}

	// Add items
	for (auto* bb : cachedItems) {
		displayBox->add(*bb);
	}

	displayBox->show_all();
}

void DirectoryNavigator::navigateInto(const string& dir) {
	pathSegments.push_back(dir);
	refresh();
}

void DirectoryNavigator::navigateUp() {
	if (not pathSegments.empty()) {
		pathSegments.pop_back();
		refresh();
	}
}

void DirectoryNavigator::navigateHome() {
	pathSegments.clear();
	refresh();
}

void DirectoryNavigator::navigateToLevel(size_t level) {
	if (level < pathSegments.size()) {
		pathSegments.resize(level + 1);
		refresh();
	}
}

string DirectoryNavigator::getCurrentPath() const {
	return Defaults::implode(pathSegments, "/");
}

bool DirectoryNavigator::isAtRoot() const {
	return pathSegments.empty();
}

Storage::BoxButton& DirectoryNavigator::createDirectoryButton(const string& dirName) {
	StringUMap rawData{
		{NAME, dirName},
		{PATH, getCurrentPath()}
	};

	auto* entry = new Storage::DirectoryEntry(rawData);
	Storage::BoxButton& btn = directoryEntries.create(entry);

	// Add clickable navigation button at the start
	auto* navButton = Gtk::make_managed<Gtk::Button>("📁 " + dirName);
	navButton->set_relief(Gtk::RELIEF_NONE);
	navButton->signal_clicked().connect([this, dirName]() {
		navigateInto(dirName);
	});
	btn.pack_start(*navButton, true, true);
	btn.reorder_child(*navButton, 0);

	addDirectoryButtons(btn, entry->createUniqueId());
//	btn.updateLabel();
	return btn;
}

void DirectoryNavigator::addDirectoryButtons(Storage::BoxButton& boxButton, const string& fullPath) {
	auto editButton = Gtk::make_managed<Gtk::Button>();
	boxButton.pack_start(*editButton, Gtk::PACK_SHRINK);
	editButton->set_image_from_icon_name("document-edit-symbolic", Gtk::ICON_SIZE_BUTTON);
	editButton->signal_clicked().connect([this, fullPath]() {
		showRenameDirectoryDialog(fullPath);
	});

	auto deleteButton = Gtk::make_managed<Gtk::Button>();
	boxButton.pack_start(*deleteButton, Gtk::PACK_SHRINK);
	deleteButton->set_image_from_icon_name("edit-delete-symbolic", Gtk::ICON_SIZE_BUTTON);
	deleteButton->signal_clicked().connect([this, fullPath]() {
		confirmDeleteDirectory(fullPath);
	});

	boxButton.show_all();
}

void DirectoryNavigator::updateBreadcrumb() {
	for (auto& widget : breadcrumbWidgets) {
		breadcrumbBox->remove(*widget);
	}
	breadcrumbWidgets.clear();

	if (pathSegments.empty()) {
		return;
	}

	for (size_t i = 0; i < pathSegments.size(); ++i) {
		bool isLast = (i == pathSegments.size() - 1);

		if (isLast) {
			auto lbl = std::make_unique<Gtk::Label>(pathSegments[i]);
			lbl->get_style_context()->add_class("BreadcrumbCurrent");
			breadcrumbBox->pack_start(*lbl, false, false);
			breadcrumbWidgets.push_back(std::move(lbl));
		}
		else {
			size_t targetLevel = i;
			auto btn = std::make_unique<Gtk::Button>(pathSegments[i]);
			btn->get_style_context()->add_class("BreadcrumbButton");
			btn->set_relief(Gtk::RELIEF_NONE);
			btn->signal_clicked().connect([this, targetLevel]() {
				navigateToLevel(targetLevel);
			});
			breadcrumbBox->pack_start(*btn, false, false);
			breadcrumbWidgets.push_back(std::move(btn));

			auto sep = std::make_unique<Gtk::Label>("/");
			sep->get_style_context()->add_class("BreadcrumbSeparator");
			breadcrumbBox->pack_start(*sep, false, false);
			breadcrumbWidgets.push_back(std::move(sep));
		}
	}

	breadcrumbBox->show_all();
}

void DirectoryNavigator::showNewDirectoryDialog() {
	Gtk::Dialog dialog("New Folder", true);
	dialog.set_transient_for(*dynamic_cast<Gtk::Window*>(displayBox->get_toplevel()));
	dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Create", Gtk::RESPONSE_OK);

	auto* content = dialog.get_content_area();
	content->set_spacing(10);
	content->set_margin_start(10);
	content->set_margin_end(10);
	content->set_margin_top(10);
	content->set_margin_bottom(10);

	Gtk::Label label("Folder name:");
	content->pack_start(label, false, false);

	Gtk::Entry entry;
	entry.set_activates_default(true);
	content->pack_start(entry, false, false);

	dialog.set_default_response(Gtk::RESPONSE_OK);
	dialog.show_all_children();

	if (dialog.run() == Gtk::RESPONSE_OK) {
		string name = Defaults::sanitizeFilename(entry.get_text());
		if (not name.empty()) {
			// Build full path for this empty dir
			string currentPath = getCurrentPath();
			string fullPath = currentPath.empty() ? name : currentPath + "/" + name;

			// Track as empty directory
			emptyDirs.insert(fullPath);

			// Add to cached dirs and refresh display
			cachedDirs.insert(name);
			populateDisplay();

			Defaults::markDirty();
		}
	}
}

void DirectoryNavigator::showRenameDirectoryDialog(const string& path) {
	auto lastSlash = path.rfind('/');
	string currentName = (lastSlash == string::npos) ? path : path.substr(lastSlash + 1);
	string parentPath  = (lastSlash == string::npos) ? "" : path.substr(0, lastSlash);

	Gtk::Dialog dialog("Rename Folder", true);
	dialog.set_transient_for(*dynamic_cast<Gtk::Window*>(displayBox->get_toplevel()));
	dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Rename", Gtk::RESPONSE_OK);

	auto* content = dialog.get_content_area();
	content->set_spacing(10);
	content->set_margin_start(10);
	content->set_margin_end(10);
	content->set_margin_top(10);
	content->set_margin_bottom(10);

	Gtk::Label label("New name:");
	content->pack_start(label, false, false);

	Gtk::Entry entry;
	entry.set_text(currentName);
	entry.set_activates_default(true);
	content->pack_start(entry, false, false);

	dialog.set_default_response(Gtk::RESPONSE_OK);
	dialog.show_all_children();

	if (dialog.run() == Gtk::RESPONSE_OK) {
		string newName = Defaults::sanitizeFilename(entry.get_text());
		if (not newName.empty() and newName != currentName) {
			string newPath = parentPath.empty() ? newName : parentPath + "/" + newName;
			renameDirectory(path, newPath);
		}
	}
}

void DirectoryNavigator::renameDirectory(const string& oldPath, const string& newPath) {
	// Update items under this path
	for (auto* bb : getItemsUnderPath(oldPath)) {
		string itemPath = bb->getData()->createUniqueId();
		string newItemPath = newPath + itemPath.substr(oldPath.length());

		// Update item's PATH property
		auto lastSlash = newItemPath.rfind('/');
		string newDir = (lastSlash == string::npos) ? "" : newItemPath.substr(0, lastSlash);
		bb->getData()->setProperty(PATH, newDir);
		bb->updateLabel();
	}

	// Update emptyDirs - rename any that start with oldPath
	std::set<string> updatedEmptyDirs;
	for (const auto& dir : emptyDirs) {
		if (dir == oldPath) {
			updatedEmptyDirs.insert(newPath);
		}
		else if (dir.find(oldPath + "/") == 0) {
			updatedEmptyDirs.insert(newPath + dir.substr(oldPath.length()));
		}
		else {
			updatedEmptyDirs.insert(dir);
		}
	}
	emptyDirs = std::move(updatedEmptyDirs);

	// Update current path if inside renamed directory
	string currentPath = getCurrentPath();
	if (currentPath == oldPath or currentPath.find(oldPath + "/") == 0) {
		string newCurrentPath = newPath + currentPath.substr(oldPath.length());
		pathSegments = Defaults::explode(newCurrentPath, '/');
	}

	Defaults::markDirty();
	refresh();
}

void DirectoryNavigator::confirmDeleteDirectory(const string& path) {
	auto itemsToDelete = getItemsUnderPath(path);
	size_t count = itemsToDelete.size();

	string msg = count == 0
		? "Delete empty folder '" + path + "'?"
		: "Delete folder '" + path + "' and " + std::to_string(count) +
		  " item" + (count == 1 ? "" : "s") + " inside?";

	if (Message::ask(msg) == Gtk::RESPONSE_YES) {
		deleteDirectory(path);
	}
}

void DirectoryNavigator::deleteDirectory(const string& path) {
	// Delete all items under this path
	for (auto* bb : getItemsUnderPath(path)) {
		items->remove(*bb);
	}

	// Remove from emptyDirs - this dir and any subdirs
	for (auto it = emptyDirs.begin(); it != emptyDirs.end(); ) {
		if (*it == path or it->find(path + "/") == 0) {
			it = emptyDirs.erase(it);
		}
		else {
			++it;
		}
	}

	// Navigate up if inside deleted directory
	string currentPath = getCurrentPath();
	if (currentPath == path or currentPath.find(path + "/") == 0) {
		auto lastSlash = path.rfind('/');
		if (lastSlash == string::npos) {
			pathSegments.clear();
		}
		else {
			pathSegments = Defaults::explode(path.substr(0, lastSlash), '/');
		}
	}

	Defaults::markDirty();
	refresh();
}

vector<Storage::BoxButton*> DirectoryNavigator::getItemsUnderPath(const string& path) const {
	vector<Storage::BoxButton*> result;
	string prefix = path + "/";

	for (auto* bb : *items) {
		string itemPath = bb->getData()->createUniqueId();
		if (itemPath.find(prefix) == 0) {
			result.push_back(bb);
		}
	}

	return result;
}
