#include "Restrictor.hpp"

using namespace LEDSpicerUI::Ui::Storage;

const string Restrictor::createPrettyName() const noexcept {
	string
		name(getValue(NAME)),
		r(Defaults::restrictorsInfo.at(name).name);
	if (Defaults::isIdUser(name, false))
		r += " Id: " + getValue(ID);
	if (Defaults::isSerial(name, false))
		r += " Port: " + getValue(PORT, "<autodetect>");
	return r;
}

const string Restrictor::createUniqueId() const noexcept {
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
