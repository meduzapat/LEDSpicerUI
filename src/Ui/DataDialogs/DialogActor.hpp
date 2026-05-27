/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DialogActor.hpp
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

#include "Storage/Actor.hpp"
#include "DialogFormHost.hpp"
#include "DialogColors.hpp"

#pragma once

namespace LEDSpicerUI::Ui::DataDialogs {

/**
 * LEDSpicerUI::Ui::DataDialogs::DialogActor
 *
 * Type-switched dialog for a single Actor inside an Animation file.
 * The type selector combo drives which family / per-type boxes are visible
 * via the AnimationFlags + AnimationInfo metadata declared in Defaults.
 *
 * Visibility convention: all widgets are hidden in onEmpty, then onSelected
 * shows only what the active type uses. No reliance on no-show-all.
 */
class DialogActor :
	public DialogFormHost,
	public SingletonDialog<DialogActor>
{

	friend class Gtk::Builder;

public:

	inline static const string noActor{"Select Actor Type"};

	virtual ~DialogActor() = default;

	void load(DataMap& values)    noexcept override;
	void clearForm()              noexcept override;
	void resetForm()              noexcept override;
	void storeData()              noexcept override;
	void retrieveData()           noexcept override;
	void isValid()          const          override;
	string createUniqueId() const noexcept override;

protected:

	// Common (Actor) — always visible when a type is selected.
	Gtk::ComboBoxText* comboBoxActorGroup  = nullptr;
	Gtk::ComboBox*     comboBoxActorFilter = nullptr;

	// Optional timers — each toggle gates the matching spinner sensitivity.
	Gtk::ToggleButton* toggleActorStartTime    = nullptr;
	Gtk::SpinButton*   spinActorStartTime      = nullptr;
	Gtk::ToggleButton* toggleActorEndTime      = nullptr;
	Gtk::SpinButton*   spinActorEndTime        = nullptr;
	Gtk::ToggleButton* toggleActorRestartTime  = nullptr;
	Gtk::SpinButton*   spinActorRestartTime    = nullptr;

	// Repeat — Repeat-with-count XOR Forever (-1).
	Gtk::ToggleButton* toggleActorRepeat        = nullptr;
	Gtk::ToggleButton* toggleActorRepeatForever = nullptr;
	Gtk::SpinButton*   spinActorRepeat          = nullptr;

	// Frame family (ANIM_USES_FRAME).
	Gtk::ToggleButton* toggleActorStartAt = nullptr;
	Gtk::Scale*        scaleActorStartAt  = nullptr;
	Gtk::ToggleButton* toggleActorCycles  = nullptr;
	Gtk::SpinButton*   spinActorCycles    = nullptr;

	// Direction family (ANIM_USES_DIRECTION) — also reused for audio with relabelled radios.
	Gtk::Box*          boxActorDirection  = nullptr;
	Gtk::RadioButton*  radioActorForward  = nullptr;
	Gtk::RadioButton*  radioActorBackward = nullptr;
	Gtk::ToggleButton* switchActorBouncer = nullptr;

	// Audio family (ANIM_USES_AUDIO).
	Gtk::Box*          boxActorAudio              = nullptr;
	Gtk::ComboBoxText* comboBoxActorAudioMode     = nullptr;
	Gtk::ComboBoxText* comboBoxActorAudioChannel  = nullptr;
	Gtk::Entry*        entryActorAlsaPcm          = nullptr;
	Gtk::Button*       btnActorAudioOff           = nullptr;
	Gtk::Button*       btnActorAudioLow           = nullptr;
	Gtk::Button*       btnActorAudioMid           = nullptr;
	Gtk::Button*       btnActorAudioHigh          = nullptr;

	// Generic color section (driven by ANIM_HAS_COLOR / ANIM_HAS_COLORS).
	Gtk::Box*          boxActorColorOptions = nullptr;
	Gtk::RadioButton*  radioBoxActorColor   = nullptr;
	Gtk::RadioButton*  radioBoxActorColors  = nullptr;
	Gtk::Button*       btnActorColor        = nullptr;
	Gtk::Box*          boxActorColors       = nullptr;
	Gtk::FlowBox*      boxActorColorList    = nullptr;
	Gtk::Button*       btnActorColorsPicker = nullptr;

	// Per-type containers (one visible at a time).
	Gtk::Box* boxActorFiller     = nullptr;
	Gtk::Box* boxActorPulse      = nullptr;
	Gtk::Box* boxActorGradient   = nullptr;
	Gtk::Box* boxActorSerpentine = nullptr;
	Gtk::Box* boxActorFileReader = nullptr;

	// Filler-specific.
	Gtk::ComboBoxText* comboBoxActorFillerMode = nullptr;

	// Pulse-specific.
	Gtk::ComboBoxText* comboBoxActorPulseMode = nullptr;

	// Gradient-specific.
	Gtk::ComboBoxText* comboBoxActorGradientMode = nullptr;
	Gtk::SpinButton*   spinActorGradientTones    = nullptr;

	// Serpentine-specific.
	Gtk::Switch*       switchActorSerpentineTail         = nullptr;
	Gtk::Box*          boxActorSerpentineTail            = nullptr;
	Gtk::SpinButton*   spinActorSerpentineTailLength     = nullptr;
	Gtk::Button*       btnActorSerpentineTailColor       = nullptr;
	Gtk::Scale*        scaleActorSerpentineTailIntensity = nullptr;

	// FileReader-specific.
	Gtk::FileChooserButton* btnActorFileReaderFile = nullptr;

	// Type brief description.
	Gtk::Label* brief = nullptr;

	/// Per-type-container visibility map. Types absent from this map
	/// (Random, AlsaAudio, PulseAudio) have no unique container.
	std::unordered_map<string, Gtk::Box*> perTypeBoxes;

	DialogActor(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) noexcept;

	const string& getType()             const noexcept override { return TYPE_ACTOR; }
	Storage::Data* createData(Values& rawData) const noexcept override;

	void onEmpty()    noexcept override;
	void onSelected() noexcept override;

};

} // namespace
