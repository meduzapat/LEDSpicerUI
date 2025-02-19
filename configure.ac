dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)

COPYRIGHT="Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)"
AC_INIT([LEDSpicerUI],[0.0.1],[https://github.com/meduzapat/LEDSpicerUI/issues],[ledspicerui],[https://github.com/meduzapat/LEDSpicerUI], [src/Message.cpp])

AC_DEFINE_UNQUOTED([PACKAGE_DATA_VERSION], ["1.0"], [Data files version])
AC_DEFINE_UNQUOTED([PACKAGE_DATA_NAME], ["LEDSpicer"], [Data files name])

AC_COPYRIGHT($COPYRIGHT)
AC_SUBST([COPYRIGHT], ["$COPYRIGHT"])
AC_SUBST([DESCRIPTION], ["$DESCRIPTION"])
AC_SUBST([MINIMUM_COMPATIBILITY], ["0.6.3 - 0.6.4"])

AC_CONFIG_SRCDIR([src/LEDSpicerUI.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])

dnl *******************
dnl * Needed Programs *
dnl *******************
AC_MSG_CHECKING([Programs])

AC_PROG_CXX
AC_PROG_INSTALL

dnl ********************
dnl * Needed Libraries *
dnl ********************
AC_MSG_CHECKING([dependencies])

PKG_CHECK_MODULES(
	[GTKMM30], [gtkmm-3.0 >= 3.20],
	[],
	[AC_MSG_ERROR([gtkmm-3.0 development library not found.])]
)

PKG_CHECK_MODULES([TINYXML2], [tinyxml2 >= 6.0],
	[],
	AC_MSG_ERROR([tinyxml2 not found.])
)

dnl expand bindir on configuration files.
AS_AC_EXPAND(BINDIR, $bindir)
AC_CONFIG_FILES([
	Makefile
	data/main.glade
])
AC_OUTPUT

dnl Dump the build configuration for the developer
AC_MSG_NOTICE([
--------- build environment -----------
Program       : $PACKAGE_NAME
Version       : $PACKAGE_VERSION
Compatibility : $MINIMUM_COMPATIBILITY
WebSite       : $PACKAGE_URL
--------- build settings -----------
C++ Compiler : $CXX $CPPFLAGS $CXXFLAGS
Includes  : $GTKMM30_CFLAGS
Libraries : $GTKMM30_LIBS
Prefix    : $prefix
bin dir   : $bindir
Data dir  : $datadir
])