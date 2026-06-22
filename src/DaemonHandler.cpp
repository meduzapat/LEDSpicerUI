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

using namespace LEDSpicerUI;
using namespace LEDSpicerUI::Ui;
using namespace LEDSpicerUI::Constants;
using LEDSpicerUI::Config::Settings;
using LEDSpicerUI::Config::ConfigFile;
using LEDSpicerUI::Config::ProjectFile;

DaemonHandler DaemonHandler::instance;

string DaemonHandler::readProc(const string& pid, const string& node) noexcept {
	try {
		return Glib::file_get_contents("/proc/" + pid + "/" + node);
	}
	catch (const Glib::Error&) {
		return {};
	}
}

bool DaemonHandler::runSystemctl(const string& action, const string& unit, bool user) noexcept {
	string out;
	return Defaults::runCommand(
		string("systemctl ") + (user ? "--user " : "") + action + ' ' + Glib::shell_quote(unit), out
	);
}

void DaemonHandler::init(
	std::function<Values()> settings,
	const Ui::Storage::BoxButtonCollection& devices,
	const Ui::Storage::BoxButtonCollection& restrictors,
	const Ui::Storage::BoxButtonCollection& groups
) noexcept {
	configProvider    = std::move(settings);
	this->devices     = &devices;
	this->restrictors = &restrictors;
	this->groups      = &groups;
}

bool DaemonHandler::connect() noexcept {
	discover();
	if (found.status != DaemonStatus::None) {
		// Only take over a daemon we may signal (signal 0 sends nothing).
		errno = 0;
		::kill(static_cast<pid_t>(std::atol(found.pid.c_str())), 0);
		if (errno == EPERM) {
			StatusBar::getInstance().push(
				"Another daemon is running and cannot be managed.", StatusBar::Severity::Warning
			);
			found = {};
			return false;
		}
		if (not stopPrior()) {
			restorePrior();
			found = {};
			return false;
		}
		tookOver = true;
	}

	const string cfg {buildTestConfig()};
	if (cfg.empty() or not start(cfg)) {
		if (tookOver)
			restorePrior();
		found = {};
		tookOver = false;
		return false;
	}
	connected = true;
	return true;
}

void DaemonHandler::disconnect() noexcept {
	if (connected) {
		string out;
		Defaults::runCommand("killall -15 " + Glib::shell_quote(processName()), out);
		waitUntil([this] { return not running(); }, STOP_TIMEOUT_MS);
		if (tookOver)
			restorePrior();
	}
	found    = {};
	tookOver = false;

	std::error_code ec;
	if (not testConfigPath.empty())  std::filesystem::remove(testConfigPath,  ec);
	if (not testProfilePath.empty()) std::filesystem::remove(testProfilePath, ec);
	connected = false;
}

void DaemonHandler::discover() noexcept {
	found = {};
	string out;
	if (not Defaults::runCommand("pidof " + Glib::shell_quote(processName()), out))
		return;
	found.pid = Defaults::explode(out, ' ').front();
	// Default to a manual launch (restore by command line); override when the leaf cgroup is a *.service (restore by unit).
	auto args = Defaults::explode(readProc(found.pid, "cmdline"), '\0');
	// Strip the -f because it need to run detached.
	args.erase(std::remove(args.begin(), args.end(), "-f"), args.end());
	found.restore = Defaults::implode(args, ' ');
	for (const string& line : Defaults::explode(readProc(found.pid, "cgroup"), '\n')) {
		string path;
		if (line.rfind("0::", 0) == 0)                                          // cgroup v2
			path = line.substr(3);
		else if (const auto p {line.find(":name=systemd:")}; p != string::npos) // cgroup v1
			path = line.substr(p + 14);
		else
			continue;
		const auto slash {path.rfind('/')};
		const string leaf {slash == string::npos ? path : path.substr(slash + 1)};
		if (leaf.size() > 8 and leaf.compare(leaf.size() - 8, 8, ".service") == 0) {
			found.status  = (path.find("user.slice") != string::npos or path.find("user@") != string::npos)
			              ? DaemonStatus::UserService : DaemonStatus::SystemService;
			found.restore = leaf;
		}
		break;
	}
}

string DaemonHandler::buildTestConfig() noexcept {
	const string projectDir {Settings::get().getProjectDir()};
	if (projectDir.empty()) {
		StatusBar::getInstance().push(
			"Open a project before connecting.", StatusBar::Severity::Warning
		);
		return emptyString;
	}

	try {
		// Empty profile so the daemon boots a blank board.
		testProfilePath = projectDir + PATH_PROFILE + RESERVED_TEST_PROFILE + ".xml";
		Values background;
		background.setValue(BACKGROUND_COLOR, DEFAULT_PROFILE_BACKGROUND_COLOR);
		string profileXml {XMLHelper::xmlHeader(TYPE_PROFILE, background)};
		Defaults::reduceTab();
		profileXml += XMLHelper::xmlFooter();
		ProjectFile::saveFile(testProfilePath, profileXml);

		// Throwaway config: keep the hardware-defining data (devices, groups),
		// strip or override the rest — debug logging, the empty profile, the
		// default loop, no processes.
		Values settings {configProvider()};
		settings.setValue("logLevel", "Debug");
		if (const string& p {settings.getValue("port")}; not p.empty())
			port = p;
		const Ui::Storage::BoxButtonCollection noProcesses;
		testConfigPath = Glib::build_filename(Glib::get_tmp_dir(), "ledspicerui-test.conf");
		ConfigFile::save(ConfigFile::ConfigData(
			testConfigPath,
			Settings::get().getCurrentProject(),
			RESERVED_TEST_PROFILE,
			DEFAULT_RUNEVERY,
			settings,
			*devices,
			*restrictors,
			*groups,
			noProcesses
		));
		return testConfigPath;
	}
	catch (Message& e) {
		StatusBar::getInstance().push(
			"Cannot connect: " + e.takeMessage(), StatusBar::Severity::Error
		);
		return emptyString;
	}
}

bool DaemonHandler::running() const noexcept {
	string out;
	return Defaults::runCommand("pidof " + Glib::shell_quote(processName()), out);
}

bool DaemonHandler::waitUntil(const std::function<bool()>& done, int timeoutMs) const noexcept {
	auto context {Glib::MainContext::get_default()};
	for (int waited {0}; waited < timeoutMs and not done(); waited += POLL_MS) {
		while (context->pending())   // keep the busy spinner animating
			context->iteration(false);
		g_usleep(POLL_MS * 1000);
	}
	return done();
}

bool DaemonHandler::stopPrior() noexcept {
	switch (found.status) {
	case DaemonStatus::UserService:   runSystemctl("stop", found.restore, true);  break;
	case DaemonStatus::SystemService: runSystemctl("stop", found.restore, false); break;
	case DaemonStatus::Manual: {
		// Ask it politely to quit.
		string out;
		Defaults::runCommand("kill -TERM " + Glib::shell_quote(found.pid), out);
		break;
	}
	default: break;
	}

	return waitUntil([this] { return not running(); }, STOP_TIMEOUT_MS);
}

void DaemonHandler::restorePrior() noexcept {
	switch (found.status) {
	case DaemonStatus::UserService:   runSystemctl("start", found.restore, true);  break;
	case DaemonStatus::SystemService: runSystemctl("start", found.restore, false); break;
	case DaemonStatus::Manual:
		// Relaunch detached.
		try {
			Glib::spawn_command_line_async(found.restore);
		}
		catch (const Glib::Error&) {}
		break;
	default: break;
	}
}

bool DaemonHandler::start(const string& configPath) const noexcept {
	const string& binary {Settings::get().getBinaryPath()};

	// Validate the config (and binary) without claiming the hardware.
	string out;
	if (not Defaults::runCommand(
		Glib::shell_quote(binary) + " -d -c " + Glib::shell_quote(configPath), out
	))
		return false;

	try {
		Glib::spawn_async(
			"",
			std::vector<std::string>{binary, "-c", configPath},
			Glib::SPAWN_SEARCH_PATH
		);
	}
	catch (const Glib::Error&) {
		return false;
	}

	// Wait for it to come up before reporting success.
	return waitUntil([this] { return running(); }, START_TIMEOUT_MS);
}

void DaemonHandler::command(Command type, std::initializer_list<string> fields) noexcept {
	if (not connected)
		return;

	const int portNumber {std::atoi(port.c_str())};
	if (portNumber <= 0 or portNumber > 65535) {
		StatusBar::getInstance().push(
			"Invalid daemon port '" + port + "'.", StatusBar::Severity::Error
		);
		return;
	}

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
			static_cast<guint16>(portNumber)
		)};
		socket->send_to(address, payload.data(), payload.size());
	}
	catch (const Glib::Error&) {
		StatusBar::getInstance().push(
			"Could not send the test command to the daemon.", StatusBar::Severity::Error
		);
	}
}

string DaemonHandler::processName() noexcept {
	const string& binary {Settings::get().getBinaryPath()};
	return binary.empty() ? string("ledspicerd") : Glib::path_get_basename(binary);
}
