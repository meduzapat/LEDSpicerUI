
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =                \
	-I$(top_srcdir)/src      \
	-I$(top_srcdir)/src/Ui   \
	-I$(srcdir)/Ui \
	-DPACKAGE_SAMPLES_DIR=\"$(abs_top_srcdir)/tests/\" \
	-fexceptions

AM_CXXFLAGS = -Wall -Wextra -std=c++17

AM_LDFLAGS = -Wl,--no-as-needed

check_PROGRAMS = DefaultsTest DataTest MessageTest XMLHelperTest

DefaultsTest_SOURCES =     \
	DefaultsTest.cpp       \
	$(top_srcdir)/src/Defaults.cpp
DefaultsTest_CPPFLAGS = \
	$(AM_CPPFLAGS)      \
	$(GTKMM30_CFLAGS)   \
	$(GTEST_CFLAGS)
DefaultsTest_LDADD = \
	$(GTKMM30_LIBS)  \
	$(GTEST_LIBS)

DataTest_SOURCES =          \
	Ui/Storage/DataTest.cpp \
	$(top_srcdir)/src/Ui/Storage/Data.cpp \
	$(top_srcdir)/src/Defaults.cpp
DataTest_CPPFLAGS =    \
	$(AM_CPPFLAGS)     \
	$(GTKMM30_CFLAGS)  \
	$(GTEST_CFLAGS)
DataTest_LDADD =     \
	$(GTKMM30_LIBS)  \
	$(GTEST_LIBS)

MessageTest_SOURCES =      \
	Ui/MessageTest.cpp     \
	$(top_srcdir)/src/Ui/Message.cpp \
	$(top_srcdir)/src/Defaults.cpp
MessageTest_CPPFLAGS =     \
	$(AM_CPPFLAGS)         \
	$(GTKMM30_CFLAGS)      \
	$(GTEST_CFLAGS)
MessageTest_LDADD =        \
	$(GTKMM30_LIBS)        \
	$(GTEST_LIBS)

XMLHelperTest_SOURCES =    \
	XMLHelperTest.cpp      \
	$(top_srcdir)/src/Ui/Message.cpp \
	$(top_srcdir)/src/Defaults.cpp   \
	$(top_srcdir)/src/XMLHelper.cpp
XMLHelperTest_CPPFLAGS =   \
	$(AM_CPPFLAGS)         \
	$(GTKMM30_CFLAGS)      \
	$(TINYXML2_CFLAGS)     \
	$(GTEST_CFLAGS)
XMLHelperTest_LDADD =  \
	$(GTKMM30_LIBS)    \
	$(TINYXML2_LIBS)   \
	$(GTEST_LIBS)

TESTS = DefaultsTest DataTest MessageTest XMLHelperTest
TESTS_ENVIRONMENT = GTEST_COLOR=1

EXTRA_DIST =           \
	data/animation.xml \
	data/config.xml    \
	data/input.xml     \
	data/invalidDataVersion.xml   \
	data/invalidLedspicerNode.xml \
	data/invalid.xml   \
	data/malformed.xml \
	data/profile.xml   \
	data/test.ui

# Clean up
DISTCLEANFILES = \
	*~
