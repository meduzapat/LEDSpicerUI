/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DaemonHandler.cpp
 * @since     Jun 19, 2026
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

#include "DaemonHandler.hpp"
#include "config/Settings.hpp"

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Config::Settings;

DaemonHandler DaemonHandler::instance;

bool DaemonHandler::connect(
	const string& configPath,
	const string& projectsDir,
	const string& port
) noexcept {
	// Always start from a clean slate; bail if a daemon refuses to stop.
	if (not disconnect())
		return false;

	this->port = port;
	const string& binary {Settings::get().getBinaryPath()};
	try {
		Glib::spawn_async(
			"",
			std::vector<std::string>{binary, "-c", configPath, "-J", projectsDir},
			Glib::SPAWN_SEARCH_PATH
		);
	}
	catch (const Glib::Error&) {
		return false;
	}

	// Wait for it to come up before reporting success.
	return waitUntil([this] { return isRunning(); }, START_TIMEOUT_MS);
}

bool DaemonHandler::disconnect() noexcept {
	// Nothing to stop: skip killall (and its "no process found" stderr).
	if (not isRunning())
		return true;
	string out;
	Defaults::runCommand("killall -15 " + Glib::shell_quote(processName()), out);
	return waitUntil([this] { return not isRunning(); }, STOP_TIMEOUT_MS);
}

bool DaemonHandler::isRunning() const noexcept {
	string out;
	return Defaults::runCommand("pidof " + Glib::shell_quote(processName()), out);
}

bool DaemonHandler::waitUntil(const std::function<bool()>& done, int timeoutMs) const noexcept {
	auto context {Glib::MainContext::get_default()};
	for (int waited {0}; waited < timeoutMs and not done(); waited += POLL_MS) {
		while (context->pending())   // keep the busy spinner animating
			context->iteration(false);
		Glib::usleep(POLL_MS * 1000);
	}
	return done();
}

bool DaemonHandler::command(Command type, std::initializer_list<string> fields) noexcept {
	// The owner gates testing: offline drops the command, stale refreshes first.
	if (not isReady())
		return false;

	const int portNumber {std::atoi(port.c_str())};
	if (portNumber <= 0 or portNumber > 65535)
		return false;

	// Wire format mirrors the daemon's Message::toString(): data fields, then
	// flags, then type, each terminated by RECORD_SEPARATOR.
	std::stringstream ss;
	for (const auto& field : fields)
		ss << field << RECORD_SEPARATOR;
	ss << 0 << RECORD_SEPARATOR << static_cast<int>(type) << RECORD_SEPARATOR;
	const string payload {ss.str()};

	try {
		auto socket {Gio::Socket::create(
			Gio::SOCKET_FAMILY_IPV4,
			Gio::SOCKET_TYPE_DATAGRAM,
			Gio::SOCKET_PROTOCOL_UDP
		)};
		auto address {Gio::InetSocketAddress::create(
			Gio::InetAddress::create_loopback(Gio::SOCKET_FAMILY_IPV4),
			static_cast<uint16_t>(portNumber)
		)};
		socket->send_to(address, payload.data(), payload.size());
	}
	catch (const Glib::Error&) {
		return false;
	}
	return true;
}

string DaemonHandler::processName() noexcept {
	const string& binary {Settings::get().getBinaryPath()};
	return binary.empty() ? DAEMON_BINARY : Glib::path_get_basename(binary);
}
