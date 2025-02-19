/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Standalone.hpp
 * @since     Feb 9, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "Data.hpp"

#ifndef STANDALONE_HPP_
#define STANDALONE_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Standalone
 */
class Standalone: public Data {

public:

	Standalone() = delete;

	Standalone(const string& fileName, unordered_map<string, string>& data) : Data(data), filename(fileName) {}

	virtual ~Standalone() = default;

protected:

	/// Stores the filename with any sub-directory, relative to the parent dir.
	string filename;

};

} /* namespace */


#endif /* STANDALONE_HPP_ */
