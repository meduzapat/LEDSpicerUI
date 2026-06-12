/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LEDSpicerUI.cpp
 * @since     Feb 12, 2023
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

#include "LEDSpicerUI.hpp"

using namespace LEDSpicerUI;

int main(int argc, char *argv[]) {

	auto app     = Gtk::Application::create(argc, argv, "org.ledspicer.ui");
	auto builder = Gtk::Builder::create();

	try {
		builder->add_from_resource("/org/ledspicer/ui/main.glade");
	}
	catch(Glib::Error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch(...) {
		std::cerr << "Fail to load interface" << std::endl;
		return EXIT_FAILURE;
	}

	// Initialize and apply theme (loads settings defaults; Settings::load() may re-apply later).
	auto& tm = Ui::ThemeManager::getInstance();
	tm.initialize();
	if (not tm.apply(Config::Settings::get().getThemeName(), Config::Settings::get().getThemeStyle()))
		std::cerr << "Warning: theme '" << Config::Settings::get().getThemeName() << "' could not be applied; running unstyled." << std::endl;

	Config::Geometry::get().load();
	Ui::MainWindow* mw = nullptr;
	builder->get_widget_derived("MainWindow", mw);
	int r = app->run(*mw);

	delete mw;

	std::cout << "LEDSpicer UI ended" << std::endl;
	return r;
}
