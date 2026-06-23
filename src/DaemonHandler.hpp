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

#include "Storage/BoxButtonCollection.hpp"
#include "config/ConfigFile.hpp"
#include "config/ProjectFile.hpp"

#pragma once

namespace LEDSpicerUI {

/**
 * LEDSpicerUI::DaemonHandler
 *
 * Manual connection to the LEDSpicer daemon for live layout testing.
 * connect() discovers any running daemon once, takes it over (stopping it
 * politely, via systemctl when systemd-managed) and launches ours with a
 * throwaway config; disconnect() stops ours and restores the one we found.
 * We only ever touch a daemon we are allowed to signal. While connected,
 * command() sends daemon messages as UDP datagrams.
 */
class DaemonHandler {

public:

	static DaemonHandler& getInstance() noexcept { return instance; }

	DaemonHandler(const DaemonHandler&)            = delete;
	DaemonHandler& operator=(const DaemonHandler&) = delete;

	/**
	 * Binds the open project's data.
	 * @param settings yields the current <settings> values.
	 * @param devices live device collection.
	 * @param restrictors live restrictor collection (for future restrictor tests).
	 * @param groups live group collection.
	 */
	void init(
		std::function<Values()> settings,
		const Ui::Storage::BoxButtonCollection& devices,
		const Ui::Storage::BoxButtonCollection& restrictors,
		const Ui::Storage::BoxButtonCollection& groups
	) noexcept;

	/**
	 * Takes over any running daemon and starts ours with the test config.
	 * @return true if our daemon is up.
	 */
	bool connect() noexcept;

	/**
	 * Stops our daemon, restores the one we took over, and removes the
	 * throwaway test files.
	 */
	void disconnect() noexcept;

	/**
	 * @return true while connected (our daemon is up).
	 */
	bool isConnected() const noexcept { return connected; }

	/**
	 * Flags the connected daemon as needing a refresh before its next command.
	 * No-op while disconnected.
	 */
	void markStale() noexcept;

	/**
	 * Installs the hooks wrapping an in-place refresh: begin() before it starts
	 * (raise the busy spinner, disable the toggle, deactivate the layout), and
	 * end(ok) after (lower the spinner, reflect disconnect on failure).
	 */
	void setRefreshHandlers(
		std::function<void()>     begin,
		std::function<void(bool)> end
	) noexcept {
		onRefreshBegin = std::move(begin);
		onRefreshEnd   = std::move(end);
	}

	/// Daemon command types. Values mirror the daemon's Message::Types enum.
	enum class Command : uint8_t {
		SetElement = 5, ClearElement = 6, SetGroup = 8, ClearGroup = 9
	};

	/**
	 * Sends a command (data fields, then flags and type) while connected.
	 */
	void command(Command type, std::initializer_list<string> fields) noexcept;

private:

	/// How a discovered daemon was launched, and so how to stop/restore it.
	enum class DaemonStatus : uint8_t { None, Manual, UserService, SystemService };

	/// Identity of the daemon found at connect, frozen for the session's recovery.
	struct Discovery {
		string       pid;      /// empty ⟺ None; used to signal/stop.
		DaemonStatus status {DaemonStatus::None};
		string       restore;  /// Manual: command line; *Service: unit name.
	};

	/// Budget and step, in ms, while waiting for the daemon to start or stop.
	static constexpr int START_TIMEOUT_MS = 5000;
	static constexpr int STOP_TIMEOUT_MS  = 5000;
	static constexpr int POLL_MS          = 100;

	DaemonHandler()  = default;
	~DaemonHandler() = default;

	/**
	 * Probes once for a running daemon and how to restore it, into found.
	 */
	void discover() noexcept;

	/**
	 * Writes the throwaway profile and config.
	 * @return its path, or empty on failure.
	 */
	string buildTestConfig() noexcept;

	/**
	 * Validates (-d) and launches the daemon.
	 * @return true once it is up.
	 */
	bool start(const string& configPath) const noexcept;

	/**
	 * Rebuilds the test config and relaunches our daemon in place, leaving any
	 * taken-over daemon stopped. Clears the stale flag.
	 * @return true once ours is back up.
	 */
	bool restart() noexcept;

	/**
	 * Atomic, non-re-entrant in-place refresh, wrapped by the refresh handlers.
	 * The triggering command is dropped; the daemon comes back blank.
	 */
	void refresh() noexcept;

	/**
	 * Stops the discovered daemon and waits for the hardware to free.
	 * @return true once nothing is running.
	 */
	bool stopPrior() noexcept;

	/**
	 * Relaunches the discovered daemon.
	 */
	void restorePrior() noexcept;

	/**
	 * @return true if a daemon process is running (live check for waits).
	 */
	bool running() const noexcept;

	/**
	 * Polls done() up to timeoutMs, pumping the UI so the busy spinner animates.
	 * @return done()'s final value.
	 */
	bool waitUntil(const std::function<bool()>& done, int timeoutMs) const noexcept;

	/**
	 * @return the daemon process name, derived from the binary path.
	 */
	static string processName() noexcept;

	/**
	 * Reads /proc/<pid>/<node>, returning empty on failure.
	 */
	static string readProc(const string& pid, const string& node) noexcept;

	/**
	 * Runs systemctl (user or system) on a unit.
	 * @return true on success.
	 */
	static bool runSystemctl(const string& action, const string& unit, bool user) noexcept;

	static DaemonHandler instance;

	/// Yields the current <settings> values for the test configuration.
	std::function<Values()> configProvider;

	/// Hooks wrapping an in-place refresh (busy spinner, toggle, layout).
	std::function<void()>     onRefreshBegin;
	std::function<void(bool)> onRefreshEnd;

	/// Guards command() and restart() against re-entry while refreshing.
	bool refreshing {false};

	/// Live project collections used to build the test configuration.
	const Ui::Storage::BoxButtonCollection
		* devices     {nullptr},
		* restrictors {nullptr},
		* groups      {nullptr};

	/// Daemon port, read from the settings when the config is built.
	string port {DEFAULT_PORT};

	/// Paths of the throwaway test configuration and profile.
	string testConfigPath, testProfilePath;

	/// Daemon found at connect; restored on disconnect when we took over.
	Discovery found;

	/// True once we stopped the discovered daemon, so it must be restored.
	bool tookOver {false};

	/// True between a successful connect() and disconnect().
	bool connected {false};
};

} // namespace
