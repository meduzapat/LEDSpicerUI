#include "Parent.hpp"
#include "Revertible.hpp"

#pragma once

namespace LEDSpicerUI::Ui::Storage {

/**
 * LEDSpicerUI::Ui::Storage::Restrictor
 * Stores a hardware restrictor and its player mappings.
 */
class Restrictor : public Parent, public Revertible {

public:

	Restrictor(StringUMap& data) noexcept :
		Parent(data, COLLECTION_RESTRICTORS, {COLLECTION_RESTRICTOR_MAP}),
		Revertible(fieldsData, &children)
	{}

	virtual ~Restrictor() = default;

	const string createPrettyName() const noexcept override;
	const string createUniqueId()   const noexcept override;
	string_view getCssClass()       const noexcept override { return "RestrictorBoxButton"; }
	string_view getXmlTag()         const noexcept override { return "restrictor"; }

	void wipe()     noexcept override;
	void tearDown() noexcept override;

};

} // namespace
