/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Revertible.cpp
 * @since     Mar 2026
 * @author    Patricio A. Rossi (MeduZa)
 * ...
 */

#include "Revertible.hpp"

using namespace LEDSpicerUI::Ui::Storage;

void Revertible::registerChild(BoxButtonCollection& child) {
	children.emplace_back(&child, BoxButtonCollection{});
}

void Revertible::swap() {
	if (not fieldsData.empty() and snapFields.empty()) {
		std::swap(fieldsData, snapFields);
		for (auto& [live, snap] : children)
			live->swap(snap);
	}
}

void Revertible::restore() {
	if (snapFields.empty()) return;
	std::swap(fieldsData, snapFields);
	snapFields.clear();
	for (auto& [live, snap] : children) {
		live->swap(snap);
		snap.wipe();
	}
}

void Revertible::wipe() {
	snapFields.clear();
	for (auto& [live, snap] : children)
		snap.wipe();
	Data::wipe();
}

void Revertible::deActivate() {
	restore();
	Data::deActivate();
}
