/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogDirectory.hpp
 * @since     Mar 2, 2026
 * @author    Patricio A. Rossi (MeduZa)
 * ...
 */

#include "DialogForm.hpp"
#include "Storage/DirectoryEntry.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogDirectory
 * Handles creation and renaming of directory nodes.
 * Operates on the current DirectoryEntry's contents collection.
 * Never serializes — directories are runtime-only.
 */
class DialogDirectory : public DialogForm, public SingletonDialog<DialogDirectory> {

	friend class Gtk::Builder;

public:

	virtual ~DialogDirectory() = default;

	void isValid() const override;
	void storeData() override;
	void retrieveData() override;
	void clearForm() override;
	const string createUniqueId() const override;
	const string getType() const override;

	Storage::CollectionHandler* getCollectionHandler() const override;

//	void setParent(Storage::DirectoryEntry* parent);

protected:

	/// Directory name input.
	Gtk::Entry* entryDirectoryName = nullptr;

	Storage::Data* createData(StringUMap& rawData) override;

};

} // namespace
