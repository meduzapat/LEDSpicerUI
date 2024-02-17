/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      NameOnly.hpp
 * @since     Apr 6, 2023
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

#include "Data.hpp"

#ifndef NAMEONLY_HPP_
#define NAMEONLY_HPP_ 1

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::NameOnly
 *
 * A very generic container for data that allows some customization
 * to avoid creating specialized classes for very common data types.
 */
class NameOnly: public Data {

public:

	using textGeneratorFunction = std::function<const string(const unordered_map<string, string>&)>;

	NameOnly() = default;

	NameOnly(
		unordered_map<string, string>& data,
		const string& node     = "",
		const string& cssClass = "",
		textGeneratorFunction prettyNamedFunction = nullptr,
		textGeneratorFunction tooltipFunction     = nullptr
	) :
		Data(data),
		node(node),
		cssClass(cssClass),
		prettyNamedFunction(prettyNamedFunction),
		tooltipFunction(tooltipFunction) {}

	virtual ~NameOnly() = default;

	const string getCssClass() const override;

	const string createTooltip() const override;

	const string createPrettyName() const override;

	const string createUniqueId() const override;

	const string toXML() const override;

protected:

	const string
		cssClass,
		node;

	textGeneratorFunction prettyNamedFunction;

	textGeneratorFunction tooltipFunction;

};

} /* namespace */


#endif /* NAMEONLY_HPP_ */
