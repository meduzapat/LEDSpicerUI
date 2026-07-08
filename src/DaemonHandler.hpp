/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DaemonHandler.hpp
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

#include "Defaults.hpp"

#pragma once

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::DaemonHandler
 *
 * Minimal link to the LEDSpicer daemon for live layout testing. connect()
 * stops any running daemon and launches ours against a caller-supplied test
 * configuration; disconnect() stops it. While connected, command() mimics the
 * emitter and sends daemon messages as UDP datagrams.
 */
class DaemonHandler {

public:

	static DaemonHandler& getInstance() noexcept { return instance; }

	DaemonHandler(const DaemonHandler&)            = delete;
	DaemonHandler& operator=(const DaemonHandler&) = delete;

	/**
	 * Stops any running daemon and launches ours with the test configuration.
	 * @param configPath  test configuration to run (daemon -c).
	 * @param projectsDir isolated projects base (daemon -J).
	 * @param port        UDP port command() will address.
	 * @return true once our daemon is up.
	 */
	bool connect(const string& configPath, const string& projectsDir, const string& port) noexcept;

	/**
	 * Stops our daemon.
	 * @return true once nothing is running, false on timeout.
	 */
	bool disconnect() noexcept;

	/// Daemon command types. Values mirror the daemon's Message::Types enum.
	enum class Command : uint8_t {
		SetElement = 5, ClearElement = 6, SetGroup = 8, ClearGroup = 9
	};

	/**
	 * Sends a command (data fields, then flags and type) to the daemon.
	 * @return true if the datagram was sent.
	 */
	bool command(Command type, std::initializer_list<string> fields) noexcept;

	/**
	 * Installs the readiness gate: command() sends only when it returns true,
	 * letting the owner refuse testing while offline and refresh a stale daemon
	 * first.
	 * @param gate predicate consulted before each send.
	 */
	void setReadyGate(std::function<bool()> gate) noexcept { readyGate = std::move(gate); }

private:

	/// Readiness gate consulted by command() before each send; installed by the owner.
	std::function<bool()> readyGate;

	/// Budget and step, in ms, while waiting for the daemon to start or stop.
	static constexpr int START_TIMEOUT_MS = 5000;
	static constexpr int STOP_TIMEOUT_MS  = 5000;
	static constexpr int POLL_MS          = 100;

	DaemonHandler()  = default;
	~DaemonHandler() = default;

	/**
	 * Polls done() up to timeoutMs, pumping the UI so the busy spinner animates.
	 * @return done()'s final value.
	 */
	bool waitUntil(const std::function<bool()>& done, int timeoutMs) const noexcept;

	/**
	 * @return true if a daemon process is running.
	 */
	bool isRunning() const noexcept;

	/**
	 * @return the daemon process name, derived from the binary path.
	 */
	static string processName() noexcept;

	static DaemonHandler instance;

	/// Daemon UDP port, set at connect and used by command().
	string port {DEFAULT_PORT};
};

} // namespace
