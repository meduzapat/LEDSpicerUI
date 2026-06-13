/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StatusBar.cpp
 * @since     Jun 4, 2026
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

#include "StatusBar.hpp"

using namespace LEDSpicerUI::Ui;

StatusBar StatusBar::instance;

/**
 * Binds the bar widget and reaches into GtkStatusbar's message area to
 * ellipsize the internal label — long messages must never widen the window.
 */
void StatusBar::initialize(const Glib::RefPtr<Gtk::Builder>& builder) noexcept {

	builder->get_widget("StatusBar", instance.bar);
	instance.contextId = instance.bar->get_context_id("main");

	auto box   {static_cast<Gtk::Box*>(instance.bar->get_message_area())};
	auto label {static_cast<Gtk::Label*>(box->get_children().front())};
	instance.label = label;
	label->set_ellipsize(Pango::ELLIPSIZE_END);
	label->set_hexpand(true);
	label->set_xalign(0.0f);
}

void StatusBar::push(const string& message, Severity severity, bool persistent) noexcept {
	if (persistent) {
		persistentText     = message;
		persistentSeverity = severity;
		hasPersistent      = true;
		if (not showingTransient)
			renderPersistent();
		return;
	}

	queue.push_back({message, severity});
	if (not showingTransient)
		showNext();
}

void StatusBar::clear() noexcept {
	timeoutConn.disconnect();
	queue.clear();
	if (showingTransient) {
		bar->pop(contextId);
		showingTransient = false;
	}
	renderPersistent();
}

unsigned StatusBar::durationFor(const string& msg, Severity sev) noexcept {
	// Count UTF-8 characters, not bytes, so non-ASCII messages don't over-count.
	const unsigned chars {static_cast<unsigned>(Glib::ustring(msg).length())};
	unsigned ms = std::clamp(chars * MS_PER_CHAR, MIN_MS, MAX_MS);
	if (sev == Severity::Warning or sev == Severity::Error)
		ms *= SEVERE_FACTOR;
	return ms;
}

void StatusBar::showNext() noexcept {
	if (queue.empty()) {
		showingTransient = false;
		renderPersistent();
		return;
	}
	const auto msg = queue.front();
	queue.pop_front();
	displayTransient(msg);
}

void StatusBar::displayTransient(const PendingMessage& msg) noexcept {
	bar->pop(contextId);
	bar->push(msg.text, contextId);
	applySeverity(msg.severity);
	showingTransient = true;
	timeoutConn.disconnect();
	timeoutConn = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &StatusBar::onTimeout),
		durationFor(msg.text, msg.severity));
}

void StatusBar::renderPersistent() noexcept {
	bar->pop(contextId);
	if (hasPersistent) {
		bar->push(persistentText, contextId);
		applySeverity(persistentSeverity);
	}
	else
		applySeverity(Severity::Info);
}

void StatusBar::applySeverity(Severity severity) noexcept {
	auto ctx = label->get_style_context();
	for (const auto& cls : SEVERITY_CLASSES)
		ctx->remove_class(cls);
	ctx->add_class(SEVERITY_CLASSES[static_cast<size_t>(severity)]);
}

bool StatusBar::onTimeout() noexcept {
	showingTransient = false;
	showNext();
	return false;
}
