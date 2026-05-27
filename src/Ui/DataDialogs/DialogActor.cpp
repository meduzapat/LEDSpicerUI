/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogActor.cpp
 * @since     May 2026
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

#include "DialogActor.hpp"

using namespace LEDSpicerUI::Ui::DataDialogs;

namespace {

inline bool typeHas(const string& type, uint8_t flag) noexcept {
	auto it {LEDSpicerUI::Defaults::animationsInfo.find(type)};
	return it != LEDSpicerUI::Defaults::animationsInfo.end() and (it->second.flags & flag);
}

} // namespace

DialogActor::DialogActor(
	BaseObjectType* obj,
	const Glib::RefPtr<Gtk::Builder>& builder
) noexcept :
	DialogFormHost(obj, builder)
{

	Gtk::Button* btnAddActor = nullptr;
	builder->get_widget_derived(
		"BoxActors",
		box,
		"BtnAnimationActorUp",
		"BtnAnimationActorDn",
		"BtnAnimationActorFirst",
		"BtnAnimationActorLast"
	);
	builder->get_widget("ComboBoxActorSelectType", selectorCombo);
	builder->get_widget("BtnAddActor",             btnAddActor);
	builder->get_widget("BtnApplyActor",           btnApply);
	builder->get_widget("BriefActor",              brief);

	// Common (Actor).
	builder->get_widget("ComboBoxActorGroup",  comboBoxActorGroup);
	builder->get_widget("ComboBoxActorFilter", comboBoxActorFilter);

	// Optional timers.
	builder->get_widget("ToggleActorStartTime",   toggleActorStartTime);
	builder->get_widget("SpinActorStartTime",     spinActorStartTime);
	builder->get_widget("ToggleActorEndTime",     toggleActorEndTime);
	builder->get_widget("SpinActorEndTime",       spinActorEndTime);
	builder->get_widget("ToggleActorRestartTime", toggleActorRestartTime);
	builder->get_widget("SpinActorRestartTime",   spinActorRestartTime);

	// Repeat.
	builder->get_widget("ToggleActorRepeat",        toggleActorRepeat);
	builder->get_widget("ToggleActorRepeatForever", toggleActorRepeatForever);
	builder->get_widget("SpinActorRepeat",          spinActorRepeat);

	// Frame family.
	builder->get_widget("ToggleActorStartAt", toggleActorStartAt);
	builder->get_widget("SpinActorStartAt",   scaleActorStartAt);
	builder->get_widget("ToggleActorCycles",  toggleActorCycles);
	builder->get_widget("SpinActorCycles",    spinActorCycles);

	// Direction family.
	builder->get_widget("BoxActorDirection",  boxActorDirection);
	builder->get_widget("RadioActorForward",  radioActorForward);
	builder->get_widget("RadioActorBackward", radioActorBackward);
	builder->get_widget("SwitchActorBouncer", switchActorBouncer);

	// Audio family.
	builder->get_widget("BoxActorAudio",             boxActorAudio);
	builder->get_widget("ComboBoxActorAudioMode",    comboBoxActorAudioMode);
	builder->get_widget("ComboBoxActorAudioChannel", comboBoxActorAudioChannel);
	builder->get_widget("EntryActorAlsaPcm",         entryActorAlsaPcm);
	builder->get_widget("BtnActorAudioOff",          btnActorAudioOff);
	builder->get_widget("BtnActorAudioLow",          btnActorAudioLow);
	builder->get_widget("BtnActorAudioMid",          btnActorAudioMid);
	builder->get_widget("BtnActorAudioHigh",         btnActorAudioHigh);

	// Generic color section.
	builder->get_widget("BoxActorColorOptions", boxActorColorOptions);
	builder->get_widget("RadioBoxActorColor",   radioBoxActorColor);
	builder->get_widget("RadioBoxActorColors",  radioBoxActorColors);
	builder->get_widget("BtnActorColor",        btnActorColor);
	builder->get_widget("BoxActorColors",       boxActorColors);
	builder->get_widget("BoxActorColorList",    boxActorColorList);
	builder->get_widget("BtnActorColorsPicker", btnActorColorsPicker);

	// Per-type containers.
	builder->get_widget("BoxActorFiller",     boxActorFiller);
	builder->get_widget("BoxActorPulse",      boxActorPulse);
	builder->get_widget("BoxActorGradient",   boxActorGradient);
	builder->get_widget("BoxActorSerpentine", boxActorSerpentine);
	builder->get_widget("BoxActorFileReader", boxActorFileReader);

	// Per-type widgets.
	builder->get_widget("BtnActorFileReaderFile",    btnActorFileReaderFile);
	builder->get_widget("ComboBoxActorFillerMode",   comboBoxActorFillerMode);
	builder->get_widget("ComboBoxActorPulseMode",    comboBoxActorPulseMode);
	builder->get_widget("ComboBoxActorGradientMode", comboBoxActorGradientMode);
	builder->get_widget("SpinActorGradientTones",    spinActorGradientTones);
	builder->get_widget("SwitchActorSerpentineTail", switchActorSerpentineTail);
	builder->get_widget("BoxActorSerpentineTail",    boxActorSerpentineTail);
	builder->get_widget("SpinActorSerpentineTailLength",     spinActorSerpentineTailLength);
	builder->get_widget("BtnActorSerpentineTailColor",       btnActorSerpentineTailColor);
	builder->get_widget("ScaleActorSerpentineTailIntensity", scaleActorSerpentineTailIntensity);

	// Per-type visibility map (only types with a unique container are listed).
	perTypeBoxes = {
		{ANIM_TYPE_FILLER,      boxActorFiller},
		{ANIM_TYPE_PULSE,       boxActorPulse},
		{ANIM_TYPE_GRADIENT,    boxActorGradient},
		{ANIM_TYPE_SERPENTINE,  boxActorSerpentine},
		{ANIM_TYPE_FILE_READER, boxActorFileReader},
	};

	setSignalAdd(btnAddActor);
	setSignalApply();

	// Wire color buttons to the picker.
	auto colors {DialogColors::getInstance()};
	colors->activateColorButton(btnActorColor);
	colors->activateColorButton(btnActorAudioOff);
	colors->activateColorButton(btnActorAudioLow);
	colors->activateColorButton(btnActorAudioMid);
	colors->activateColorButton(btnActorAudioHigh);
	colors->activateColorButton(btnActorSerpentineTailColor);

	colors->activateColorPicker(btnActorColorsPicker, boxActorColorList);

	// Toggle-gates-spinner sensitivity for every optional field.
	Defaults::linkToggleToWidget(toggleActorStartTime,   spinActorStartTime);
	Defaults::linkToggleToWidget(toggleActorEndTime,     spinActorEndTime);
	Defaults::linkToggleToWidget(toggleActorRestartTime, spinActorRestartTime);
	Defaults::linkToggleToWidget(toggleActorStartAt,     scaleActorStartAt);
	Defaults::linkToggleToWidget(toggleActorCycles,      spinActorCycles);
	Defaults::linkToggleToWidget(toggleActorRepeat,      toggleActorRepeatForever->get_parent());
	Defaults::linkToggleToWidget(toggleActorRepeatForever, spinActorRepeat, true);

	// Forever, when active, additionally grays out the times spinner.
	toggleActorRepeat->signal_toggled().connect([this]() {
		toggleActorRepeatForever->set_active(false);
	});

	toggleActorRepeatForever->signal_toggled().connect([this]() {
		spinActorRepeat->set_value(0.00f);
	});

	Defaults::linkSwitchToWidget(switchActorSerpentineTail, boxActorSerpentineTail);

	// Color options: single vs multiple.
	radioBoxActorColor->signal_toggled().connect([this]() {
		const bool single {radioBoxActorColor->get_active()};
		btnActorColor->get_parent()->set_sensitive(single);
		boxActorColors->set_sensitive(not single);
		// Wipe the now-inactive picker.
		auto colors {DialogColors::getInstance()};
		if (single)
			colors->wipeColorPicker(boxActorColorList);
		else
			colors->colorizeButton(btnActorColor, NO_COLOR);
	});

	// Populate selector.
	initializeSelector(noActor, Defaults::animationsInfo);

	selectorCombo->signal_changed().connect([this]() {
		if (handleTypeSwitch(
			getBox(),
			"Are you sure you want to change the actor type? Type-specific fields will be reset."
		)) {
			resetForm();
		}
	});
}

void DialogActor::load(DataMap& values) noexcept {
	createItems(
		values[Defaults::createCommonUniqueId({ownerData->getProperties().getValue(PATH_BASE), COLLECTION_ACTORS})],
		values
	);
}

LEDSpicerUI::Ui::Storage::Data* DialogActor::createData(Values& rawData) const noexcept {
	return new Storage::Actor(rawData, ownerData->getProperties().getValue(UID));
}

string DialogActor::createUniqueId() const noexcept {
	return currentData ? currentData->createUniqueId() : emptyString;
}

void DialogActor::resetForm() noexcept {

	DialogForm::resetForm();

	const string type {selectorCombo->get_active_id()};
	if (type.empty()) return;

	const auto flags {Defaults::animationsInfo.at(type).flags};
	const bool
		usesFrame     {static_cast<bool>(flags & Defaults::ANIM_USES_FRAME)},
		usesDirection {static_cast<bool>(flags & Defaults::ANIM_USES_DIRECTION)},
		usesAudio     {static_cast<bool>(flags & Defaults::ANIM_USES_AUDIO)},
		hasColor      {static_cast<bool>(flags & Defaults::ANIM_HAS_COLOR)},
		hasColors     {static_cast<bool>(flags & Defaults::ANIM_HAS_COLORS)};

	// Frame-family widgets (StartAt and Cycles share a row).
	toggleActorStartAt->get_parent()->set_visible(usesFrame);

	// Direction also shows for audio with relabelled radios; bouncer doesn't.
	boxActorDirection->set_visible(usesDirection or usesAudio);
	switchActorBouncer->set_visible(usesDirection);

	if (usesAudio) {
		radioActorForward->set_label(DIRECTION_OUTWARD);
		radioActorBackward->set_label(DIRECTION_INWARD);
	}
	else {
		radioActorForward->set_label(DIRECTION_FORWARD);
		radioActorBackward->set_label(DIRECTION_BACKWARD);
	}

	// Audio family. PCM row only applies to ALSA.
	boxActorAudio->set_visible(usesAudio);
	if (usesAudio)
		entryActorAlsaPcm->get_parent()->set_visible(type == ANIM_TYPE_ALSA_AUDIO);

	// Colors.
	boxActorColorOptions->set_visible(hasColor or hasColors);
	radioBoxActorColor->get_parent()->set_visible(hasColor and hasColors);
	btnActorColor->get_parent()->set_visible(hasColor);
	boxActorColors->set_visible(hasColors);
	if (not hasColor)  radioBoxActorColors->set_active(true);
	if (not hasColors) radioBoxActorColor->set_active(true);

	// Per-type container.
	if (auto it = perTypeBoxes.find(type); it != perTypeBoxes.end())
		it->second->set_visible(true);

	brief->set_text(Defaults::animationsInfo.at(type).brief);
	btnApply->set_sensitive(true);
}

void DialogActor::isValid() const {

	const string type {selectorCombo->get_active_id()};
	if (type.empty()) throw Message("Select an actor type.");

	const auto info {Defaults::animationsInfo.find(type)};
	if (info == Defaults::animationsInfo.end())
		throw Message("Unknown actor type: " + type);

	const auto flags {info->second.flags};

	if (comboBoxActorGroup->get_active_text().empty())
		throw Message("Pick a group for the actor.");

	if (comboBoxActorFilter->get_active_id().empty())
		throw Message("Pick a filter.");

	// Numeric fields. Widgets clamp on user input but raw XML can hold garbage.
	if (action == Actions::LOAD) {
		const auto checkNum = [this](const string& key) {
			const string v {currentData->getValue(key)};
			if (not v.empty() and not Defaults::isNumber(v))
				throw Message("Invalid numeric value for " + key + ": " + v);
		};
		checkNum(START_TIME);
		checkNum(END_TIME);
		checkNum(RESTART_TIME);
		checkNum(START_AT);
		checkNum(CYCLES);
		checkNum(TAIL_LENGTH);
		checkNum(TAIL_INTENSITY);
		checkNum(TONES);

		const string repeat {currentData->getValue(REPEAT)};
		if (not repeat.empty() and repeat != "-1" and not Defaults::isNumber(repeat))
			throw Message("Invalid repeat value: " + repeat);

		const string bouncer {currentData->getValue(BOUNCER)};
		if (not bouncer.empty() and bouncer != HUMAN_TRUE and bouncer != HUMAN_FALSE)
			throw Message("Invalid bouncer value: " + bouncer);

		if (const string dir {currentData->getValue(DIRECTION)};
		    not dir.empty() and dir != DIRECTION_FORWARD and dir != DIRECTION_BACKWARD)
			throw Message("Invalid direction value: " + dir);
	}

	// Color requirements.
	const bool hasColor  {static_cast<bool>(flags & Defaults::ANIM_HAS_COLOR)};
	const bool hasColors {static_cast<bool>(flags & Defaults::ANIM_HAS_COLORS)};
	const bool useColors {hasColors and (not hasColor or radioBoxActorColors->get_active())};

	if (hasColor and not useColors and btnActorColor->get_label().empty())
		throw Message("Pick a color.");

	if (useColors) {
		const auto vals {DialogColors::getInstance()->getColorBoxValues(boxActorColorList)};
		const size_t minColors {type == ANIM_TYPE_GRADIENT ? 3u : 1u};
		if (vals.size() < minColors)
			throw Message(
				type + " needs at least " + std::to_string(minColors) + " color"
				+ (minColors == 1 ? "" : "s") + "."
			);
	}

	// Audio palette: every band must have a color.
	if (flags & Defaults::ANIM_USES_AUDIO) {
		for (auto* btn : {btnActorAudioOff, btnActorAudioLow, btnActorAudioMid, btnActorAudioHigh})
			if (btn->get_label().empty())
				throw Message("All audio colors (Off / Low / Mid / High) must be set.");
	}

	if (type == ANIM_TYPE_FILE_READER and btnActorFileReaderFile->get_filename().empty())
		throw Message("Pick a binary file to play back.");
}

void DialogActor::storeData() noexcept {

	const string type {selectorCombo->get_active_id()};

	currentData->setValue(TYPE,        type);
	currentData->setValue(ACTOR_GROUP, comboBoxActorGroup->get_active_text().raw());
	currentData->setValue(FILTER,      comboBoxActorFilter->get_active_id());

	auto storeOptional = [this](const string& key, Gtk::ToggleButton* tog, Gtk::SpinButton* spin) {
		if (tog->get_active())
			currentData->setValue(key, spin->get_text());
	};
	storeOptional(START_TIME,   toggleActorStartTime,   spinActorStartTime);
	storeOptional(END_TIME,     toggleActorEndTime,     spinActorEndTime);
	storeOptional(RESTART_TIME, toggleActorRestartTime, spinActorRestartTime);

	// Repeat: master gates everything. Forever (only meaningful when Repeat is on)
	if (toggleActorRepeat->get_active()) {
		if (toggleActorRepeatForever->get_active())
			currentData->setValue(REPEAT, "-1");
		else
			currentData->setValue(REPEAT, spinActorRepeat->get_text());
	}

	if (typeHas(type, Defaults::ANIM_USES_FRAME)) {
		if (toggleActorStartAt->get_active())
			currentData->setValue(START_AT, std::to_string(static_cast<int>(scaleActorStartAt->get_value())));
		if (toggleActorCycles->get_active())
			currentData->setValue(CYCLES, spinActorCycles->get_text());
	}

	if (typeHas(type, Defaults::ANIM_USES_DIRECTION) or typeHas(type, Defaults::ANIM_USES_AUDIO))
		currentData->setValue(DIRECTION, radioActorBackward->get_active() ? DIRECTION_BACKWARD : DIRECTION_FORWARD);

	if (typeHas(type, Defaults::ANIM_USES_DIRECTION) and switchActorBouncer->get_active())
		currentData->setValue(BOUNCER, HUMAN_TRUE);


	auto colors {DialogColors::getInstance()};

	// Generic color storage based on the type's flags.
	const auto flags {Defaults::animationsInfo.at(type).flags};
	const bool
		hasColor {static_cast<bool>(flags & Defaults::ANIM_HAS_COLOR)},
		hasMulti {static_cast<bool>(flags & Defaults::ANIM_HAS_COLORS)},
		useMulti {hasMulti and (not hasColor or radioBoxActorColors->get_active())};

	if (hasColor and not useMulti) {
		const string c {btnActorColor->get_label()};
		if (not c.empty()) currentData->setValue(COLOR, c);
	}
	if (useMulti) {
		const auto vals {colors->getColorBoxValues(boxActorColorList)};
		if (not vals.empty())
			currentData->setValue(COLORS, Defaults::implode(vals, ','));
	}

	if (type == ANIM_TYPE_FILLER)
		currentData->setValue(MODE, comboBoxActorFillerMode->get_active_id());

	if (type == ANIM_TYPE_PULSE)
		currentData->setValue(MODE, comboBoxActorPulseMode->get_active_id());

	if (type == ANIM_TYPE_GRADIENT) {
		currentData->setValue(MODE, comboBoxActorGradientMode->get_active_id());
		currentData->setValue(TONES, spinActorGradientTones->get_text());
	}

	if (type == ANIM_TYPE_SERPENTINE and switchActorSerpentineTail->get_active()) {
		currentData->setValue(TAIL_LENGTH, spinActorSerpentineTailLength->get_text());
		currentData->setValue(TAIL_INTENSITY, std::to_string(static_cast<int>(scaleActorSerpentineTailIntensity->get_value())));
		const string tailColor {btnActorSerpentineTailColor->get_label()};
		if (not tailColor.empty())
			currentData->setValue(TAIL_COLOR, tailColor);
	}

	if (type == ANIM_TYPE_FILE_READER) {
		currentData->setValue(FILENAME, btnActorFileReaderFile->get_filename());
		currentData->setValue(FORMAT,   FORMAT_RGBA);
	}

	if (type == ANIM_TYPE_ALSA_AUDIO or type == ANIM_TYPE_PULSE_AUDIO) {
		currentData->setValue(MODE,    comboBoxActorAudioMode->get_active_id());
		currentData->setValue(CHANNEL, comboBoxActorAudioChannel->get_active_id());
		auto store = [this](const string& key, Gtk::Button* btn) {
			const string col {btn->get_label()};
			if (not col.empty())
				currentData->setValue(key, col);
		};
		store(OFF,  btnActorAudioOff);
		store(LOW,  btnActorAudioLow);
		store(MID,  btnActorAudioMid);
		store(HIGH, btnActorAudioHigh);
		const string audioPCM {entryActorAlsaPcm->get_text()};
		if (type == ANIM_TYPE_ALSA_AUDIO and not audioPCM.empty())
			currentData->setValue(PCM, audioPCM);
	}
}

void DialogActor::retrieveData() noexcept {

	const string type {currentData->getValue(TYPE)};

	// previousName MUST be set before set_active_id triggers signal_changed.
	previousName = type;
	selectorCombo->set_active_id(type);
	comboBoxActorGroup->set_active_text(currentData->getValue(ACTOR_GROUP));
	if (const auto v = currentData->getValue(FILTER); not v.empty())
		comboBoxActorFilter->set_active_id(v);

	auto loadOptional = [this](const string& key, Gtk::ToggleButton* tog, Gtk::SpinButton* spin) {
		const string v {currentData->getValue(key)};
		const bool on {not v.empty()};
		tog->set_active(on);
		if (on) spin->set_value(std::stod(v));
	};
	loadOptional(START_TIME,   toggleActorStartTime,   spinActorStartTime);
	loadOptional(END_TIME,     toggleActorEndTime,     spinActorEndTime);
	loadOptional(RESTART_TIME, toggleActorRestartTime, spinActorRestartTime);

	const string repeat {currentData->getValue(REPEAT)};
	if (repeat.empty() or repeat == "0") {
		toggleActorRepeat->set_active(false);
	}
	else {
		toggleActorRepeat->set_active(true);
		if (repeat == "-1")
			toggleActorRepeatForever->set_active(true);
		else
			spinActorRepeat->set_value(std::stoi(repeat));
	}

	if (typeHas(type, Defaults::ANIM_USES_FRAME)) {
		const string startAt {currentData->getValue(START_AT)};
		if (not startAt.empty()) {
			toggleActorStartAt->set_active(true);
			scaleActorStartAt->set_value(std::stod(startAt));
		}

		const string cycles {currentData->getValue(CYCLES)};
		if (not cycles.empty()) {
			toggleActorCycles->set_active(true);
			spinActorCycles->set_value(std::stoi(cycles));
		}
	}

	if (typeHas(type, Defaults::ANIM_USES_DIRECTION) or typeHas(type, Defaults::ANIM_USES_AUDIO)) {
		if (currentData->getValue(DIRECTION) == DIRECTION_BACKWARD)
			radioActorBackward->set_active(true);

		if (typeHas(type, Defaults::ANIM_USES_DIRECTION))
			switchActorBouncer->set_active(currentData->is(BOUNCER));
	}

	auto colors {DialogColors::getInstance()};

	// Generic color restoration.
	const string
		singleColor {currentData->getValue(COLOR)},
		multiColors {currentData->getValue(COLORS)};

	const bool useMulti {singleColor.empty() and not multiColors.empty()};

	// Set the radio first; its toggled signal clears the inactive picker.
	if (useMulti)
		radioBoxActorColors->set_active(true);
	else
		radioBoxActorColor->set_active(true);

	if (not singleColor.empty())
		colors->colorizeButton(btnActorColor, singleColor);

	if (not multiColors.empty())
		colors->populateColorBox(boxActorColorList, Defaults::explode(multiColors, ','));

	if (type == ANIM_TYPE_FILLER)
		comboBoxActorFillerMode->set_active_id(currentData->getValue(MODE, HUMAN_NORMAL));

	if (type == ANIM_TYPE_PULSE)
		comboBoxActorPulseMode->set_active_id(currentData->getValue(MODE, MODE_EXPONENTIAL));

	if (type == ANIM_TYPE_GRADIENT) {
		comboBoxActorGradientMode->set_active_id(currentData->getValue(MODE, MODE_ALL));
		spinActorGradientTones->set_value(std::stoi(currentData->getValue(TONES, "10")));
	}

	if (type == ANIM_TYPE_SERPENTINE) {
		const string tailLen {currentData->getValue(TAIL_LENGTH)};
		if (not tailLen.empty() and tailLen != "0") {
			switchActorSerpentineTail->set_active(true);
			spinActorSerpentineTailLength->set_value(std::stoi(tailLen));
			scaleActorSerpentineTailIntensity->set_value(std::stoi(currentData->getValue(TAIL_INTENSITY)));
			colors->colorizeButton(btnActorSerpentineTailColor, currentData->getValue(TAIL_COLOR));
		}
	}

	if (type == ANIM_TYPE_FILE_READER)
		btnActorFileReaderFile->set_filename(currentData->getValue(FILENAME));

	if (type == ANIM_TYPE_ALSA_AUDIO or type == ANIM_TYPE_PULSE_AUDIO) {
		comboBoxActorAudioMode->set_active_id(currentData->getValue(MODE));
		comboBoxActorAudioChannel->set_active_id(currentData->getValue(CHANNEL));
		colors->colorizeButton(btnActorAudioOff, currentData->getValue(OFF));
		colors->colorizeButton(btnActorAudioLow, currentData->getValue(LOW));
		colors->colorizeButton(btnActorAudioMid, currentData->getValue(MID));
		colors->colorizeButton(btnActorAudioHigh, currentData->getValue(HIGH));
		if (type == ANIM_TYPE_ALSA_AUDIO)
			entryActorAlsaPcm->set_text(currentData->getValue(PCM));
	}
}

void DialogActor::onEmpty() noexcept {

	// Optional timers.
	toggleActorStartTime->set_active(false);
	toggleActorEndTime->set_active(false);
	toggleActorRestartTime->set_active(false);

	// Repeat.
	toggleActorRepeatForever->set_active(false);
	toggleActorRepeat->set_active(false);

	// StartAt and Cycles share a row.
	toggleActorStartAt->get_parent()->set_visible(false);
	toggleActorStartAt->set_active(false);
	toggleActorCycles->set_active(false);

	// Direction + Audio family.
	boxActorDirection->set_visible(false);
	switchActorBouncer->set_visible(false);
	boxActorAudio->set_visible(false);

	// Generic color section.
	boxActorColorOptions->set_visible(false);

	// Per-type containers.
	for (auto& b : perTypeBoxes) b.second->set_visible(false);

	auto colors {DialogColors::getInstance()};

	Storage::CollectionHandler::getInstance(COLLECTION_GROUPS)->refreshComboBox(comboBoxActorGroup);
	comboBoxActorGroup->set_active(-1);
	comboBoxActorFilter->set_active_id(FILTER_COMBINE);

	spinActorStartTime->set_value(1);
	spinActorEndTime->set_value(1);
	spinActorRestartTime->set_value(1);
	spinActorRepeat->set_value(1);
	scaleActorStartAt->set_value(100);
	spinActorCycles->set_value(1);

	radioActorForward->set_active(true);
	switchActorBouncer->set_active(false);

	// Audio defaults.
	comboBoxActorAudioMode->set_active_id(MODE_VU_METER);
	comboBoxActorAudioChannel->set_active_id(CHANNEL_BOTH);
	colors->colorizeButton(btnActorAudioOff,  AUDIO_DEFAULT_OFF);
	colors->colorizeButton(btnActorAudioLow,  AUDIO_DEFAULT_LOW);
	colors->colorizeButton(btnActorAudioMid,  AUDIO_DEFAULT_MID);
	colors->colorizeButton(btnActorAudioHigh, AUDIO_DEFAULT_HIGH);
	entryActorAlsaPcm->set_text(emptyString);

	colors->colorizeButton(btnActorColor, NO_COLOR);
	colors->wipeColorPicker(boxActorColorList);
	radioBoxActorColor->set_active(true);

	comboBoxActorFillerMode->set_active_id(MODE_NORMAL);
	comboBoxActorPulseMode->set_active_id(MODE_EXPONENTIAL);
	comboBoxActorGradientMode->set_active_id(MODE_ALL);
	spinActorGradientTones->set_value(10);

	switchActorSerpentineTail->set_active(false);
	spinActorSerpentineTailLength->set_value(1);
	scaleActorSerpentineTailIntensity->set_value(75);
	colors->colorizeButton(btnActorSerpentineTailColor, NO_COLOR);

	btnActorFileReaderFile->unselect_all();

	brief->set_text("");
	btnApply->set_sensitive(false);
}

void DialogActor::onSelected() noexcept {

	const string type {selectorCombo->get_active_id()};
	if (type.empty()) return;

	// Drop fields the new type doesn't support.
	if (not typeHas(type, Defaults::ANIM_USES_FRAME)) {
		currentData->unSet(START_AT);
		currentData->unSet(CYCLES);
	}
	if (not typeHas(type, Defaults::ANIM_USES_DIRECTION))
		currentData->unSet(BOUNCER);
	if (not (typeHas(type, Defaults::ANIM_USES_DIRECTION) or typeHas(type, Defaults::ANIM_USES_AUDIO)))
		currentData->unSet(DIRECTION);
}
