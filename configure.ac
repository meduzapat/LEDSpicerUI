dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)

COPYRIGHT="Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)"
AC_INIT([LEDSpicerUI],[0.0.2],[https://github.com/meduzapat/LEDSpicerUI/issues],[ledspicerui],[https://github.com/meduzapat/LEDSpicerUI], [src/Message.cpp])

AC_DEFINE_UNQUOTED([PACKAGE_DATA_VERSION], ["1.0"], [Data files version])
AC_DEFINE_UNQUOTED([PACKAGE_DATA_NAME], ["LEDSpicer"], [Data files name])

AC_COPYRIGHT($COPYRIGHT)
AC_SUBST([COPYRIGHT], ["$COPYRIGHT"])
AC_SUBST([MINIMUM_COMPATIBILITY], ["0.6.3 - 0.6.4"])

AC_CONFIG_SRCDIR([src/LEDSpicerUI.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror])

dnl Needed Programs
AC_MSG_CHECKING([for required programs])
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Needed Libraries
AC_MSG_CHECKING([for dependencies])

PKG_CHECK_MODULES(
	[GTKMM30], [gtkmm-3.0 >= 3.24],
	[],
	[AC_MSG_ERROR([gtkmm-3.0 development library not found.])]
)

PKG_CHECK_MODULES([TINYXML2], [tinyxml2 >= 6.0],
	[],
	AC_MSG_ERROR([tinyxml2 not found.])
)

dnl Collect image files dynamically
IMAGES_FILES=`find data/images -maxdepth 1 -type f \( -name "*.png" -o -name "*.svg" \) | tr '\n' ' '`
AC_SUBST([IMAGES_FILES])

dnl Optional Google Test check
AC_MSG_CHECKING([for Google Test])
PKG_CHECK_MODULES([GTEST], [gtest >= 1.8],
	[
		AC_SUBST([GTEST_CFLAGS])
		AC_SUBST([GTEST_LIBS])
		AC_MSG_RESULT([yes])
		have_gtest=yes
	],
	[
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Google Test >= 1.8 not found; developer tests unavailable. Program functionality unaffected.])
		have_gtest=no
	])

AM_CONDITIONAL([HAVE_GTEST], [test "x$have_gtest" = "xyes"])

dnl Expand bindir for use in config files if needed.
AS_AC_EXPAND(BINDIR, $bindir)

dnl Output files.
AC_CONFIG_FILES([
	Makefile
	tests/Makefile
	data/main.glade
])

AC_OUTPUT

dnl Dump the build configuration.
AC_MSG_NOTICE([
--------- Build Environment -----------
Program       : $PACKAGE_NAME
Version       : $PACKAGE_VERSION
Compatibility : $MINIMUM_COMPATIBILITY
WebSite       : $PACKAGE_URL
--------- Build Settings -----------
C++ Compiler : $CXX $CPPFLAGS $CXXFLAGS
Includes     : $GTKMM30_CFLAGS $GTEST_CFLAGS
Libraries    : $GTKMM30_LIBS $GTEST_LIBS
Prefix       : $prefix
Bin Dir      : $bindir
Data Dir     : $datadir
Tests Enabled: $have_gtest
])