/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LEDSpicerUI.cpp
 * @since     Feb 12, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 - 2024 Patricio A. Rossi (MeduZa)
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

	auto app         = Gtk::Application::create(argc, argv, "org.ledspicer.ui");
	auto builder     = Gtk::Builder::create();
	auto cssProvider = Gtk::CssProvider::create();

	try {
		builder->add_from_file(PACKAGE_DATA_DIR       "main.glade");
		cssProvider->load_from_path(PACKAGE_DATA_DIR  "style.css");
	}
	catch(Glib::Error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch(...) {
		std::cerr << "Fail to load interface" << std::endl;
		return EXIT_FAILURE;
	}

	// Set CSS styles.
	auto styleContext = Gtk::StyleContext::create();
	styleContext->add_provider_for_screen(
		Gdk::Screen::get_default(),
		cssProvider,
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
	);

	Ui::MainWindow* mw = nullptr;
	builder->get_widget_derived("MainWindow", mw);
	Message::initialize(builder, mw);
	int r = app->run(*mw);

	delete mw;

	std::cout << "LEDSpicer UI ended" << std::endl;
	return r;
}
