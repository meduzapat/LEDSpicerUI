#include "Restrictor.hpp"

using namespace LEDSpicerUI::Ui::Storage;

const string Restrictor::createPrettyName() const noexcept {
	string
		name(fieldsData.at(NAME)),
		r(Defaults::restrictorsInfo.at(name).name);
	if (Defaults::isIdUser(name, false))
		r += " Id: " + fieldsData.at(ID);
	if (Defaults::isSerial(name, false))
		r += " Port: " + (fieldsData.at(PORT).empty() ? "<autodetect>" : fieldsData.at(PORT));
	return r;
}

const string Restrictor::createUniqueId() const noexcept {
	if (getValues()->empty()) return "";
	return Defaults::createHardwareUniqueId(*getValues(), false);
}

void Restrictor::wipe() noexcept {
	clearSnap();
	Data::wipe();
}

void Restrictor::tearDown() noexcept {
	revert();
	Data::tearDown();
}
