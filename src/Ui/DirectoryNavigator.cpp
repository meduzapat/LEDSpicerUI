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
using namespace LEDSpicerUI::Config;

DirectoryNavigator::DirectoryNavigator() noexcept :
	currentDir(&rootDir)
{}

void DirectoryNavigator::onActivate() noexcept {
	wireDialogs();
}

void DirectoryNavigator::enterDirectory(Storage::DirectoryEntry* dir) noexcept {
	currentDir = dir;
	wireDialogs();
}

void DirectoryNavigator::navigateUp() noexcept {
	if (currentDir == &rootDir) return;
	auto parent {static_cast<Storage::DirectoryEntry*>(currentDir->getParent())};
	currentDir = parent ? parent : &rootDir;
	wireDialogs();
}

bool DirectoryNavigator::isAtRoot() const noexcept {
	return currentDir == &rootDir;
}

Storage::DirectoryEntry* DirectoryNavigator::getCurrentDir() const noexcept {
	return currentDir;
}

void DirectoryNavigator::load() noexcept {
	clear();
	process(&rootDir, Settings::get().getProjectDir() + string(getSubDir()));
	wireDialogs();
}

void DirectoryNavigator::save() const noexcept {

	namespace fs = std::filesystem;
	const string baseDir {Settings::get().getProjectDir() + string(getSubDir())};

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

void DirectoryNavigator::process(Storage::DirectoryEntry* parent, const string& absPath) noexcept {

	namespace fs = std::filesystem;
	std::error_code ec;

	vector<fs::directory_entry> entries;
	for (auto& e : fs::directory_iterator(absPath, ec)) entries.push_back(e);
	std::sort(entries.begin(), entries.end());

	for (auto& entry : entries) {
		if (entry.is_directory(ec)) {
			const string name {entry.path().filename().string()};
			auto& bb {parent->createSubDir(name)};
			wireDirButtons(bb);
			process(static_cast<Storage::DirectoryEntry*>(bb.getData()), entry.path().string());
		}
		else if (entry.is_regular_file(ec) and entry.path().extension() == ".xml") {
			try {
				extractData(entry.path().string(), parent);
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

string DirectoryNavigator::promptDirName(const string& current) noexcept {

	Gtk::MessageDialog dialog("Folder Name", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL, true);
	dialog.set_transient_for(Message::getMain());
	dialog.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	Gtk::Entry entry;
	entry.set_text(current);
	entry.set_activates_default(true);
	dialog.get_message_area()->pack_start(entry, false, false);
	dialog.show_all();
	entry.grab_focus();
	if (dialog.run() != Gtk::RESPONSE_OK)
		return emptyString;
	return Defaults::sanitizeFilename(entry.get_text());
}

void DirectoryNavigator::onNewDirClicked() noexcept {

	const string name {promptDirName("")};
	if (name.empty()) return;

	const string uid {Defaults::createCommonUniqueId({currentDir->getFsId(), name})};
	if (Storage::CollectionHandler::getInstance(COLLECTION_DIRECTORIES)->isIdSet(uid)) {
		Message::displayError("A folder named \"" + name + "\" already exists here.");
		return;
	}

	auto& bb {currentDir->createSubDir(name)};
	wireDirButtons(bb);
	Defaults::markDirty();
	wireDialogs();
}

void DirectoryNavigator::wireDirButtons(Storage::BoxButton& bb) noexcept {

	auto de {static_cast<Storage::DirectoryEntry*>(bb.getData())};

	// Navigation button — takes over the label area so the whole name is clickable.
	auto navBtn {Gtk::make_managed<Gtk::Button>()};
	navBtn->set_relief(Gtk::RELIEF_NONE);
	navBtn->set_hexpand(true);
	auto lbox {static_cast<Gtk::HBox*>(bb.getLabel()->get_parent())};
	lbox->remove(*bb.getLabel());
	navBtn->add(*bb.getLabel());
	lbox->pack_start(*navBtn, Gtk::PACK_EXPAND_WIDGET);
	navBtn->signal_clicked().connect([this, de]() {
		enterDirectory(de);
	});

	// Edit button — rename the directory.
	auto editBtn {Gtk::make_managed<Gtk::Button>()};
	bb.packButtonStart(*editBtn);
	editBtn->set_image_from_icon_name(ICON_EDIT, Gtk::ICON_SIZE_BUTTON);
	editBtn->get_style_context()->add_class(CSS_BOX_BACKGROUND_EDIT);
	editBtn->set_tooltip_text("Rename " + de->createPrettyName());
	editBtn->signal_clicked().connect([this, de, &bb]() {

		const string name {promptDirName(de->getName())};
		// If the name is empty or unchanged, do nothing.
		if (name.empty() or name == de->getName()) return;

		const string uid {Defaults::createCommonUniqueId({
			de->getParent() ? de->getParent()->getFsId() : emptyString,
			name
		})};
		if (Storage::CollectionHandler::getInstance(COLLECTION_DIRECTORIES)->isIdSet(uid)) {
			Message::displayError("A folder named \"" + name + "\" already exists here.");
			return;
		}
		const string oldId {de->createUniqueId()};
		de->getProperties().setValue(FILENAME, name);
		de->syncRegistration(oldId);
		bb.sync();
		Defaults::markDirty();
	});

	// Delete button — remove the directory and all its contents.
	auto delBtn {Gtk::make_managed<Gtk::Button>()};
	bb.packButtonStart(*delBtn);
	delBtn->set_image_from_icon_name(ICON_TRASH, Gtk::ICON_SIZE_BUTTON);
	delBtn->get_style_context()->add_class(CSS_BOX_BACKGROUND_DELETE);
	delBtn->set_tooltip_text("Delete " + de->createPrettyName());
	delBtn->signal_clicked().connect([this, de, &bb]() {
		if (Message::ask(
			"Are you sure you want to remove " + de->createPrettyName() + "?"
		) != Gtk::ResponseType::RESPONSE_YES) return;
		currentDir->getPrimaryChild()->remove(bb);
		Defaults::markDirty();
		wireDialogs();
	});

	bb.show_all();
}

void DirectoryNavigator::sortDirectoriesFirst(OrdenableFlowBox* box) noexcept {
	box->set_sort_func([](Gtk::FlowBoxChild* a, Gtk::FlowBoxChild* b) -> int {
		auto
			dA {static_cast<Storage::BoxButton*>(a)->getData()},
			dB {static_cast<Storage::BoxButton*>(b)->getData()};
		const bool
			aIsDir {dynamic_cast<Storage::DirectoryEntry*>(dA) != nullptr},
			bIsDir {dynamic_cast<Storage::DirectoryEntry*>(dB) != nullptr};
		if (aIsDir != bIsDir)
			return aIsDir ? -1 : 1;
		return dynamic_cast<Storage::DirNode*>(dA)->getName().compare(
			dynamic_cast<Storage::DirNode*>(dB)->getName()
		);
	});
}

void DirectoryNavigator::wireDialogs() noexcept {

	setupDialog();

	btnHome->set_sensitive(not isAtRoot());

	// Rebuild breadcrumb.
	for (auto child : boxBreadcrumb->get_children())
		boxBreadcrumb->remove(*child);

	if (not currentDir->isAtRoot()) {
		// Walk ancestors bottom → up, prepending each so left-to-right order stays correct.
		auto node {static_cast<Storage::DirectoryEntry*>(currentDir->getParent())};
		while (not node->isAtRoot()) {
			auto btn {Gtk::make_managed<Gtk::Button>(node->getName())};
			btn->get_style_context()->add_class(CSS_BREADCRUMB_BUTTON);
			btn->signal_clicked().connect([this, node]() {
				enterDirectory(node);
			});
			auto sep {Gtk::make_managed<Gtk::Label>("/")};
			sep->get_style_context()->add_class(CSS_BREADCRUMB_SEPARATOR);

			boxBreadcrumb->pack_start(*btn, Gtk::PACK_SHRINK);
			boxBreadcrumb->pack_start(*sep, Gtk::PACK_SHRINK);
			boxBreadcrumb->reorder_child(*btn, 0);
			boxBreadcrumb->reorder_child(*sep, 1);

			node = static_cast<Storage::DirectoryEntry*>(node->getParent());
		}

		// Trailing separator + current (non-clickable) label.
		auto sep {Gtk::make_managed<Gtk::Label>("/")};
		sep->get_style_context()->add_class(CSS_BREADCRUMB_SEPARATOR);
		boxBreadcrumb->pack_end(*sep, Gtk::PACK_SHRINK);

		auto cur {Gtk::make_managed<Gtk::Label>(currentDir->getName())};
		cur->get_style_context()->add_class(CSS_BREADCRUMB_CURRENT);
		boxBreadcrumb->pack_end(*cur, Gtk::PACK_SHRINK);
	}

	boxBreadcrumb->show_all();
}
