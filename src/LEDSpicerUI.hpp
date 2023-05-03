/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LEDSpicerUI.hpp
 * @since     Feb 12, 2023
 * @author    Patricio Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

#include <iostream>

// To handle c signals.
#include <csignal>
using std::signal;

#include "Message.hpp"
#include "Ui/MainWindow.hpp"

#include <glibmm.h>

#ifndef LEDSPICERUI_HPP_
#define LEDSPICERUI_HPP_ 1

namespace LEDSpicerUI {
int main(int argc, char *argv[]);
}

#endif /* LEDSPICERUI_HPP_ */
