/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DefaultsTest.cpp
 * @since     Feb 19, 2025
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

#include <gtest/gtest.h>
#include "Defaults.hpp"

using namespace LEDSpicerUI;

TEST(DefaultsTest, IsNumber) {
	EXPECT_TRUE(Defaults::isNumber("123"));
	EXPECT_TRUE(Defaults::isNumber("0"));
	EXPECT_FALSE(Defaults::isNumber("12a3"));
	EXPECT_FALSE(Defaults::isNumber("12.3"));
	EXPECT_FALSE(Defaults::isNumber(""));
	EXPECT_FALSE(Defaults::isNumber("abc"));
}

TEST(DefaultsTest, IsBetween) {
	EXPECT_TRUE(Defaults::isBetween("5", 1, 10));
	EXPECT_TRUE(Defaults::isBetween("1", 1, 10));
	EXPECT_TRUE(Defaults::isBetween("10", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("0", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("11", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("", 1, 10));
	EXPECT_FALSE(Defaults::isBetween("abc", 1, 10));
	// Test with high = -1 (no upper bound)
	EXPECT_TRUE(Defaults::isBetween("5", 1, -1));
	EXPECT_FALSE(Defaults::isBetween("0", 1, -1));
}

TEST(DefaultsTest, AddUnitSeparator) {
	EXPECT_EQ(Defaults::addUnitSeparator("test"), UNIT_SEPARATOR + string("test") + UNIT_SEPARATOR);
	EXPECT_EQ(Defaults::addUnitSeparator(emptyString), UNIT_SEPARATOR + emptyString + UNIT_SEPARATOR);
	EXPECT_EQ(Defaults::addUnitSeparator("a b"), UNIT_SEPARATOR + string("a b") + UNIT_SEPARATOR);
}

TEST(DefaultsTest, Explode) {
	// Basic delimiter
	auto result1 = Defaults::explode("a,b,c", ',');
	EXPECT_EQ(result1.size(), 3);
	EXPECT_EQ(result1[0], "a");
	EXPECT_EQ(result1[1], "b");
	EXPECT_EQ(result1[2], "c");

	// With limit
	auto result2 = Defaults::explode("1,2,3,4", ',', 2);
	EXPECT_EQ(result2.size(), 2);
	EXPECT_EQ(result2[0], "1");
	EXPECT_EQ(result2[1], "2,3,4");

	// Empty string
	auto result3 = Defaults::explode("", ',');
	EXPECT_TRUE(result3.empty());

	// Single item
	auto result4 = Defaults::explode("item", ',');
	EXPECT_EQ(result4.size(), 1);
	EXPECT_EQ(result4[0], "item");

	// Spaces trimmed
	auto result5 = Defaults::explode("  a  , b ,  c  ", ',');
	EXPECT_EQ(result5.size(), 3);
	EXPECT_EQ(result5[0], "a");
	EXPECT_EQ(result5[1], "b");
	EXPECT_EQ(result5[2], "c");

	// Single item with spaces trimmed (no-delimiter path)
	auto result7 = Defaults::explode("  item  ", ',');
	EXPECT_EQ(result7.size(), 1);
	EXPECT_EQ(result7[0], "item");
}

TEST(DefaultsTest, MatchElementTypeByName) {
	// Direct keyword → id.
	EXPECT_EQ(ELEMENT_TYPE_BUTTON,    Defaults::matchElementTypeByName("button1"));
	EXPECT_EQ(ELEMENT_TYPE_JOYSTICK,  Defaults::matchElementTypeByName("joystick_left"));
	EXPECT_EQ(ELEMENT_TYPE_TRACKBALL, Defaults::matchElementTypeByName("p1_trackball"));
	EXPECT_EQ(ELEMENT_TYPE_SPINNER,   Defaults::matchElementTypeByName("SPINNER_1"));  // case-insensitive
	EXPECT_EQ(ELEMENT_TYPE_CREDIT,    Defaults::matchElementTypeByName("credit"));
	EXPECT_EQ(ELEMENT_TYPE_LIGHT,     Defaults::matchElementTypeByName("light_red"));
	EXPECT_EQ(ELEMENT_TYPE_BAR,       Defaults::matchElementTypeByName("healthbar"));
	EXPECT_EQ(ELEMENT_TYPE_ACTUATOR,  Defaults::matchElementTypeByName("knocker"));
	EXPECT_EQ(ELEMENT_TYPE_LIGHTGUN,  Defaults::matchElementTypeByName("P1_LIGHTGUN"));

	// Aliases.
	EXPECT_EQ(ELEMENT_TYPE_CREDIT,    Defaults::matchElementTypeByName("coin1"));
	EXPECT_EQ(ELEMENT_TYPE_BAR,       Defaults::matchElementTypeByName("LedStrip"));
	EXPECT_EQ(ELEMENT_TYPE_TRACKBALL, Defaults::matchElementTypeByName("P1_MOUSE"));
	EXPECT_EQ(ELEMENT_TYPE_SPINNER,   Defaults::matchElementTypeByName("dial_x"));
	EXPECT_EQ(ELEMENT_TYPE_SPINNER,   Defaults::matchElementTypeByName("paddle"));
	EXPECT_EQ(ELEMENT_TYPE_JOYSTICK,  Defaults::matchElementTypeByName("p1_pedal"));
	EXPECT_EQ(ELEMENT_TYPE_JOYSTICK,  Defaults::matchElementTypeByName("positional1"));
	EXPECT_EQ(ELEMENT_TYPE_ACTUATOR,  Defaults::matchElementTypeByName("solenoid_left"));
	EXPECT_EQ(ELEMENT_TYPE_ACTUATOR,  Defaults::matchElementTypeByName("motor1"));
	EXPECT_EQ(ELEMENT_TYPE_ACTUATOR,  Defaults::matchElementTypeByName("recoil_right"));

	// Joystick + restrictor-position suffix → light (not joystick).
	EXPECT_EQ(ELEMENT_TYPE_LIGHT,     Defaults::matchElementTypeByName("P1_JOYSTICK1_4WAYS"));
	EXPECT_EQ(ELEMENT_TYPE_LIGHT,     Defaults::matchElementTypeByName("P1_JOYSTICK1_8WAYS"));

	// Priority: lightgun before joystick keyword scan.
	EXPECT_EQ(ELEMENT_TYPE_LIGHTGUN,  Defaults::matchElementTypeByName("P1_LIGHTGUN_JOYSTICK"));

	// No keyword hit → empty string (caller falls back to a generic compatible).
	EXPECT_EQ(emptyString,            Defaults::matchElementTypeByName("unknown_xyz"));
}

TEST(DefaultsTest, BuildPlayerControlName) {
	// Full form with WAYS modifier.
	EXPECT_EQ("P1_JOYSTICK1_8WAYS",
	          Defaults::buildPlayerControlName("1", "JOYSTICK", "1", "8WAYS"));
	// Button, no ways.
	EXPECT_EQ("P2_BUTTON4",
	          Defaults::buildPlayerControlName("2", "BUTTON", "4", emptyString));
	// START — no index, no ways even if passed.
	EXPECT_EQ("P1_START",
	          Defaults::buildPlayerControlName("1", "START", emptyString, emptyString));
	EXPECT_EQ("P1_START",
	          Defaults::buildPlayerControlName("1", "START", "5", "anything"));
	// COIN — same rule as START.
	EXPECT_EQ("P3_COIN",
	          Defaults::buildPlayerControlName("3", "COIN", emptyString, emptyString));
	// Player missing.
	EXPECT_EQ(emptyString,
	          Defaults::buildPlayerControlName(emptyString, "JOYSTICK", "1", emptyString));
	// Type missing.
	EXPECT_EQ(emptyString,
	          Defaults::buildPlayerControlName("1", emptyString, "1", emptyString));
	// Index required and missing.
	EXPECT_EQ(emptyString,
	          Defaults::buildPlayerControlName("1", "BUTTON", emptyString, emptyString));
}

TEST(DefaultsTest, BuildCabinetItemName) {
	EXPECT_EQ("FLOOR",  Defaults::buildCabinetItemName("FLOOR", emptyString));
	EXPECT_EQ("FLOOR2", Defaults::buildCabinetItemName("FLOOR", "2"));
	EXPECT_EQ(emptyString, Defaults::buildCabinetItemName(emptyString, "1"));
	EXPECT_EQ(emptyString, Defaults::buildCabinetItemName(emptyString, emptyString));
}

TEST(DefaultsTest, ImplodeChar) {
	StringVector values{"a", "b", "c"};
	EXPECT_EQ(Defaults::implode(values, ','), "a,b,c");

	StringVector single{"item"};
	EXPECT_EQ(Defaults::implode(single, ','), "item");

	StringVector empty;
	EXPECT_EQ(Defaults::implode(empty, ','), "");

	StringVector spaced{" a ", " b ", " c "};
	EXPECT_EQ(Defaults::implode(spaced, ','), " a , b , c ");
}

TEST(DefaultsTest, ImplodeString) {
	StringVector values{"x", "y", "z"};
	EXPECT_EQ(Defaults::implode(values, "---"), "x---y---z");

	StringVector single{"test"};
	EXPECT_EQ(Defaults::implode(single, "---"), "test");

	StringVector empty;
	EXPECT_EQ(Defaults::implode(empty, "---"), "");
}

TEST(DefaultsTest, ImplodeUnorderedSet) {
	StringUSet values{"one", "two", "three"};
	string result = Defaults::implode(values, ',');

	// Use explode to split result back into a vector
	StringVector exploded(Defaults::explode(result, ','));
	StringUSet resultSet(exploded.begin(), exploded.end());

	EXPECT_EQ(resultSet, values);

	StringUSet single{"only"};
	EXPECT_EQ(Defaults::implode(single, ','), "only");

	StringUSet empty;
	EXPECT_EQ(Defaults::implode(empty, ','), "");
}

TEST(DefaultsTest, LTrim) {
	string s1 = "   text";
	Defaults::ltrim(s1);
	EXPECT_EQ(s1, "text");

	string s2 = "text";
	Defaults::ltrim(s2);
	EXPECT_EQ(s2, "text");

	string s3 = "   ";
	Defaults::ltrim(s3);
	EXPECT_EQ(s3, "");

	string s4 = "";
	Defaults::ltrim(s4);
	EXPECT_EQ(s4, "");
}

TEST(DefaultsTest, RTrim) {
	string s1 = "text   ";
	Defaults::rtrim(s1);
	EXPECT_EQ(s1, "text");

	string s2 = "text";
	Defaults::rtrim(s2);
	EXPECT_EQ(s2, "text");

	string s3 = "   ";
	Defaults::rtrim(s3);
	EXPECT_EQ(s3, "");

	string s4 = "";
	Defaults::rtrim(s4);
	EXPECT_EQ(s4, "");

	string s5 = " 1";
	Defaults::rtrim(s5);
	EXPECT_EQ(s5, " 1");

	string s6 = "  1 ";
	Defaults::rtrim(s6);
	EXPECT_EQ(s6, "  1");
}

TEST(DefaultsTest, Trim) {
	string s1 = "   text   ";
	Defaults::trim(s1);
	EXPECT_EQ(s1, "text");

	string s2 = "text";
	Defaults::trim(s2);
	EXPECT_EQ(s2, "text");

	string s3 = "   ";
	Defaults::trim(s3);
	EXPECT_EQ(s3, "");

	string s4 = "";
	Defaults::trim(s4);
	EXPECT_EQ(s4, "");
}

TEST(DefaultsTest, CreateCommonUniqueId) {
	StringVector fields{"field1", "field2", "field3"};
	string expected = string("field1") + FIELD_SEPARATOR + "field2" + FIELD_SEPARATOR + "field3";
	EXPECT_EQ(Defaults::createCommonUniqueId(fields), expected);

	StringVector single{"single"};
	EXPECT_EQ(Defaults::createCommonUniqueId(single), "single");

	StringVector empty;
	EXPECT_EQ(Defaults::createCommonUniqueId(empty), "");
}

TEST(DefaultsTest, ExtractName) {

	// Common Test cases.
	EXPECT_EQ("file",   Defaults::extractName("file",              "/a/b/c"));
	EXPECT_EQ("file",   Defaults::extractName("file.xml",          "/a/b/c"));
	EXPECT_EQ("file",   Defaults::extractName("/a/b/c/file.xml",   "/a/b/c"));
	EXPECT_EQ("d/file", Defaults::extractName("/a/b/c/d/file.xml", "/a/b/c"));
	EXPECT_EQ("file",   Defaults::extractName("/a1/b1/file.xml",   "/a/b/c"));
	// Test with deeper directory structures.
	EXPECT_EQ("d/e/f/file", Defaults::extractName("/a/b/c/d/e/f/file.xml", "/a/b/c"));
	EXPECT_EQ("file",       Defaults::extractName("/a/b/c/file",           "/a/b/c"));
	// Nasty corner cases.
	EXPECT_EQ("a/file",  Defaults::extractName("/a/file.xml",        ""));
	EXPECT_EQ("",        Defaults::extractName("",                   "/a/b/c"));
	EXPECT_EQ("file",    Defaults::extractName("/a/b/c/file.xml",    "/a/b/c/d/e/f"));
	EXPECT_EQ("file.v1", Defaults::extractName("/a/b/c/file.v1.xml", "/a/b/c"));
	EXPECT_EQ("file",    Defaults::extractName("/a/b/c/file.xml",    "/a/b/c/file.xml"));
}

TEST(DefaultsTest, AnimationsInfoShape) {
	// The eight actor types this dialog supports must be present.
	const StringVector expected {
		"Filler", "Gradient", "Pulse", "Serpentine",
		"Random", "FileReader", "AlsaAudio", "PulseAudio"
	};
	EXPECT_EQ(expected.size(), Defaults::animationsInfo.size());
	for (const auto& name : expected) {
		EXPECT_NE(
			Defaults::animationsInfo.end(),
			Defaults::animationsInfo.find(name)
		) << "Missing animation type: " << name;
	}

	// Every entry must have a non-empty name and brief description.
	for (const auto& [id, info] : Defaults::animationsInfo) {
		EXPECT_FALSE(info.name.empty())  << "Empty name for "  << id;
		EXPECT_FALSE(info.brief.empty()) << "Empty brief for " << id;
	}
}

TEST(DefaultsTest, AnimationsInfoFlagInvariants) {
	const auto& info = Defaults::animationsInfo;

	// Audio actors do NOT use the frame/direction families and DO use audio.
	for (const char* a : {"AlsaAudio", "PulseAudio"}) {
		const auto flags = info.at(a).flags;
		EXPECT_FALSE(flags & Defaults::ANIM_USES_FRAME)     << a;
		EXPECT_FALSE(flags & Defaults::ANIM_USES_DIRECTION) << a;
		EXPECT_TRUE (flags & Defaults::ANIM_USES_AUDIO)     << a;
		// Audio actors have their own 4-color palette; not regular color/colors.
		EXPECT_FALSE(flags & Defaults::ANIM_HAS_COLOR)      << a;
		EXPECT_FALSE(flags & Defaults::ANIM_HAS_COLORS)     << a;
	}

	// Frame-family actors all use the frame flag and never audio.
	for (const char* f : {"Filler", "Gradient", "Pulse", "Serpentine", "Random", "FileReader"}) {
		const auto flags = info.at(f).flags;
		EXPECT_TRUE (flags & Defaults::ANIM_USES_FRAME) << f;
		EXPECT_FALSE(flags & Defaults::ANIM_USES_AUDIO) << f;
	}

	// XOR color types — both flags set.
	for (const char* x : {"Filler", "Pulse"}) {
		const auto flags = info.at(x).flags;
		EXPECT_TRUE(flags & Defaults::ANIM_HAS_COLOR)  << x;
		EXPECT_TRUE(flags & Defaults::ANIM_HAS_COLORS) << x;
	}

	// Multi-color only.
	for (const char* m : {"Gradient", "Random"}) {
		const auto flags = info.at(m).flags;
		EXPECT_FALSE(flags & Defaults::ANIM_HAS_COLOR)  << m;
		EXPECT_TRUE (flags & Defaults::ANIM_HAS_COLORS) << m;
	}

	// Single-color only.
	EXPECT_TRUE (info.at("Serpentine").flags & Defaults::ANIM_HAS_COLOR);
	EXPECT_FALSE(info.at("Serpentine").flags & Defaults::ANIM_HAS_COLORS);

	// FileReader uses neither color slot.
	EXPECT_FALSE(info.at("FileReader").flags & Defaults::ANIM_HAS_COLOR);
	EXPECT_FALSE(info.at("FileReader").flags & Defaults::ANIM_HAS_COLORS);
}

TEST(DefaultsTest, InputHasFlag) {
	EXPECT_TRUE (Defaults::inputHasFlag("Actions", Defaults::INPUT_HAS_SPEED));
	EXPECT_TRUE (Defaults::inputHasFlag("Actions", Defaults::INPUT_NEEDS_SOURCE));
	EXPECT_FALSE(Defaults::inputHasFlag("Actions", Defaults::INPUT_HAS_TIMES));
	EXPECT_TRUE (Defaults::inputHasFlag("Blinker", Defaults::INPUT_HAS_TIMES));
	EXPECT_TRUE (Defaults::needSource("Actions"));
	EXPECT_FALSE(Defaults::needSource("Mame"));
	EXPECT_TRUE (Defaults::hasLinkedMaps("Credits"));
	EXPECT_FALSE(Defaults::hasLinkedMaps("Mame"));
}

TEST(DefaultsTest, ExtractAfter) {
	EXPECT_EQ("value",       Defaults::extractAfter("key=value",    "key="));
	EXPECT_EQ("trimmed",     Defaults::extractAfter("key=  trimmed  ", "key="));
	EXPECT_EQ("",            Defaults::extractAfter("nokey",        "key="));
	EXPECT_EQ("",            Defaults::extractAfter("",             "key="));
	// Prefix appearing mid-string still matches at first occurrence.
	EXPECT_EQ("rest",        Defaults::extractAfter("foo:: rest",   "::"));
}

TEST(DefaultsTest, SanitizeFilename) {
	EXPECT_EQ("clean",      Defaults::sanitizeFilename("clean"));
	EXPECT_EQ("",           Defaults::sanitizeFilename("/\\:*?\"<>|"));
	EXPECT_EQ("a b c",      Defaults::sanitizeFilename("a b c"));            // spaces preserved
	EXPECT_EQ("ab",         Defaults::sanitizeFilename("a/b"));
	EXPECT_EQ("filename",   Defaults::sanitizeFilename("file<>name"));
	EXPECT_EQ("",           Defaults::sanitizeFilename(""));
	// Control characters stripped.
	EXPECT_EQ("ab",         Defaults::sanitizeFilename("a\x01" "b"));
	EXPECT_EQ("ab",         Defaults::sanitizeFilename("a\x1E" "b"));        // RS separator
	EXPECT_EQ("ab",         Defaults::sanitizeFilename("a\x1F" "b"));        // US separator
	EXPECT_EQ("ab",         Defaults::sanitizeFilename("a\x7F" "b"));        // DEL
	// Leading dots stripped.
	EXPECT_EQ("",           Defaults::sanitizeFilename("."));
	EXPECT_EQ("",           Defaults::sanitizeFilename("..."));
	EXPECT_EQ("foo",        Defaults::sanitizeFilename("..foo"));
	EXPECT_EQ("a.b",        Defaults::sanitizeFilename("a.b"));              // non-leading dot preserved
	// maxLen truncation.
	EXPECT_EQ("abc",        Defaults::sanitizeFilename("abcdef", 3));
	EXPECT_EQ("abcdef",     Defaults::sanitizeFilename("abcdef", 6));
}

TEST(DefaultsTest, SanitizeName) {
	EXPECT_EQ("clean",      Defaults::sanitizeName("clean"));
	EXPECT_EQ("",           Defaults::sanitizeName(""));
	// XML-unsafe chars stripped.
	EXPECT_EQ("",           Defaults::sanitizeName("<>&\""));
	EXPECT_EQ("ab",         Defaults::sanitizeName("a<b"));
	EXPECT_EQ("ab",         Defaults::sanitizeName("a>b"));
	EXPECT_EQ("ab",         Defaults::sanitizeName("a&b"));
	EXPECT_EQ("ab",         Defaults::sanitizeName("a\"b"));
	// Control characters stripped.
	EXPECT_EQ("ab",         Defaults::sanitizeName("a\x01" "b"));
	EXPECT_EQ("ab",         Defaults::sanitizeName("a\x1E" "b"));            // RS separator
	EXPECT_EQ("ab",         Defaults::sanitizeName("a\x1F" "b"));            // US separator
	EXPECT_EQ("ab",         Defaults::sanitizeName("a\x7F" "b"));            // DEL
	// Allowed special chars preserved.
	EXPECT_EQ("a b",        Defaults::sanitizeName("a b"));                  // space
	EXPECT_EQ("it's",       Defaults::sanitizeName("it's"));                 // apostrophe
}

TEST(DefaultsTest, EscapeXmlValue) {
	EXPECT_EQ("clean",      Defaults::escapeXmlValue("clean"));
	EXPECT_EQ("",           Defaults::escapeXmlValue(""));
	EXPECT_EQ("&amp;",      Defaults::escapeXmlValue("&"));
	EXPECT_EQ("&lt;",       Defaults::escapeXmlValue("<"));
	EXPECT_EQ("&gt;",       Defaults::escapeXmlValue(">"));
	EXPECT_EQ("&quot;",     Defaults::escapeXmlValue("\""));
	EXPECT_EQ("a&amp;b",    Defaults::escapeXmlValue("a&b"));
	EXPECT_EQ("&lt;tag&gt;", Defaults::escapeXmlValue("<tag>"));
	EXPECT_EQ("say &quot;hi&quot;", Defaults::escapeXmlValue("say \"hi\""));
	// Non-special chars pass through unchanged.
	EXPECT_EQ("hello world", Defaults::escapeXmlValue("hello world"));
	EXPECT_EQ("it's",        Defaults::escapeXmlValue("it's"));
}

TEST(DefaultsTest, LinkSwitchToWidget) {
	Gtk::Switch sw;
	Gtk::Button target;

	sw.set_active(false);
	Defaults::linkSwitchToWidget(&sw, &target);
	EXPECT_FALSE(target.get_sensitive());            // mirror initial state.

	sw.set_active(true);
	EXPECT_TRUE(target.get_sensitive());

	sw.set_active(false);
	EXPECT_FALSE(target.get_sensitive());
}

TEST(DefaultsTest, LinkSwitchToWidgetInverted) {
	Gtk::Switch sw;
	Gtk::Button target;

	sw.set_active(false);
	Defaults::linkSwitchToWidget(&sw, &target, true);
	EXPECT_TRUE(target.get_sensitive());             // inverted initial state.

	sw.set_active(true);
	EXPECT_FALSE(target.get_sensitive());

	sw.set_active(false);
	EXPECT_TRUE(target.get_sensitive());
}

TEST(DefaultsTest, LinkToggleToWidget) {
	Gtk::ToggleButton toggle;
	Gtk::Button target;

	toggle.set_active(false);
	Defaults::linkToggleToWidget(&toggle, &target);
	EXPECT_FALSE(target.get_sensitive());

	toggle.set_active(true);
	EXPECT_TRUE(target.get_sensitive());

	toggle.set_active(false);
	EXPECT_FALSE(target.get_sensitive());
}

TEST(DefaultsTest, LinkToggleToWidgetInverted) {
	Gtk::ToggleButton toggle;
	Gtk::Button target;

	toggle.set_active(false);
	Defaults::linkToggleToWidget(&toggle, &target, true);
	EXPECT_TRUE(target.get_sensitive());

	toggle.set_active(true);
	EXPECT_FALSE(target.get_sensitive());

	toggle.set_active(false);
	EXPECT_TRUE(target.get_sensitive());
}

int main(int argc, char** argv) {
	auto app = Gtk::Application::create(argc, argv, "org.test");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
