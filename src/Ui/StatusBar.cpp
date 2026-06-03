/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StatusBar.cpp
 * @since     Jun 2026
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

namespace {
	constexpr unsigned MIN_MS        = 3000;
	constexpr unsigned MAX_MS        = 8000;
	constexpr unsigned MS_PER_CHAR   = 70;
	constexpr unsigned SEVERE_FACTOR = 2;
	// Minimum time to keep showing a message after the pointer leaves the bar,
	// so a brief hover near the end of its window does not make it vanish instantly.
	constexpr unsigned MIN_RESUME_MS = 250;
}

void StatusBar::initialize(const Glib::RefPtr<Gtk::Builder>& builder) noexcept {
	builder->get_widget("StatusBar", instance.bar);
	instance.contextId = instance.bar->get_context_id("main");

	// Ellipsize the internal label so long messages never widen the window.
	// GtkStatusbar's message area is a GtkBox whose first child is the GtkLabel.
	auto box = dynamic_cast<Gtk::Box*>(instance.bar->get_message_area());
	instance.label = dynamic_cast<Gtk::Label*>(box->get_children().front());
	instance.label->set_ellipsize(Pango::ELLIPSIZE_END);
	instance.label->set_hexpand(true);
	instance.label->set_xalign(0.0f);

	// Hover-pause: freeze the auto-dismiss timer while the pointer is over the bar.
	instance.bar->add_events(Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);
	instance.bar->signal_enter_notify_event().connect(
		sigc::mem_fun(instance, &StatusBar::onPointerEnter));
	instance.bar->signal_leave_notify_event().connect(
		sigc::mem_fun(instance, &StatusBar::onPointerLeave));
}

void StatusBar::push(const std::string& message, Severity severity, bool persistent) noexcept {
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
	paused = false;
	if (showingTransient) {
		bar->pop(contextId);
		showingTransient = false;
	}
	renderPersistent();
}

unsigned StatusBar::durationFor(const std::string& msg, Severity sev) noexcept {
	unsigned ms = static_cast<unsigned>(msg.size()) * MS_PER_CHAR;
	ms = std::clamp(ms, MIN_MS, MAX_MS);
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
	showingTransient  = true;
	currentDurationMs = durationFor(msg.text, msg.severity);
	displayStartUs    = g_get_monotonic_time();
	paused            = false;
	timeoutConn.disconnect();
	timeoutConn = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &StatusBar::onTimeout),
		currentDurationMs);
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
	for (const auto& cls : {"status-info", "status-success", "status-warning", "status-error"})
		ctx->remove_class(cls);
	switch (severity) {
	case Severity::Info:    ctx->add_class("status-info");    break;
	case Severity::Success: ctx->add_class("status-success"); break;
	case Severity::Warning: ctx->add_class("status-warning"); break;
	case Severity::Error:   ctx->add_class("status-error");   break;
	}
}

bool StatusBar::onTimeout() noexcept {
	showingTransient = false;
	showNext();
	return false;
}

bool StatusBar::onPointerEnter(GdkEventCrossing*) noexcept {
	if (not showingTransient or paused)
		return false;
	const gint64 elapsedMs = (g_get_monotonic_time() - displayStartUs) / 1000;
	remainingMsOnHover = elapsedMs >= currentDurationMs
		? 0
		: currentDurationMs - static_cast<unsigned>(elapsedMs);
	timeoutConn.disconnect();
	paused = true;
	return false;
}

bool StatusBar::onPointerLeave(GdkEventCrossing*) noexcept {
	if (not paused)
		return false;
	paused = false;
	const unsigned ms = std::max(remainingMsOnHover, MIN_RESUME_MS);
	currentDurationMs = ms;
	displayStartUs    = g_get_monotonic_time();
	timeoutConn = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &StatusBar::onTimeout),
		ms);
	return false;
}
