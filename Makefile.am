#################
# Automake file #
#################

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =                             \
	-I$(srcdir)/src                       \
	-I$(srcdir)/src/Ui                    \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)/\" \
	-DPACKAGE_DATA_DIR=\"$(pkgdatadir)/\" \
	-Wl,--no-as-needed                    \
	-fexceptions

bin_PROGRAMS = ledspicerui

ledspicerui_SOURCES =   \
	src/Defaults.cpp    \
	src/XMLHelper.cpp   \
	src/ConfigFile.cpp  \
	src/InputFile.cpp   \
	src/Ui/Message.cpp  \
	src/Ui/DialogColors.cpp \
	src/Ui/OrdenableFlowBox.cpp   \
	src/Ui/OrdenableListBox.cpp   \
	src/Ui/Storage/CollectionHandler.cpp \
	src/Ui/Storage/Data.cpp          \
	src/Ui/Storage/NameOnly.cpp      \
	src/Ui/Storage/Element.cpp       \
	src/Ui/Storage/Group.cpp         \
	src/Ui/Storage/Device.cpp        \
	src/Ui/Storage/RestrictorMap.cpp \
	src/Ui/Storage/Restrictor.cpp    \
	src/Ui/Storage/Process.cpp       \
	src/Ui/Storage/InputMapLink.cpp  \
	src/Ui/Storage/InputMap.cpp      \
	src/Ui/Storage/Input.cpp         \
	src/Ui/Storage/Profile.cpp       \
	src/Ui/Storage/BoxButton.cpp     \
	src/Ui/Storage/BoxButtonCollection.cpp     \
	src/Ui/DataDialogs/DialogForm.cpp          \
	src/Ui/DataDialogs/DialogSelect.cpp        \
	src/Ui/DataDialogs/DialogElement.cpp       \
	src/Ui/DataDialogs/DialogGroup.cpp         \
	src/Ui/DataDialogs/DialogDevice.cpp        \
	src/Ui/DataDialogs/DialogRestrictorMap.cpp \
	src/Ui/DataDialogs/DialogRestrictor.cpp    \
	src/Ui/DataDialogs/DialogInputLinkMaps.cpp \
	src/Ui/DataDialogs/DialogInputMap.cpp      \
	src/Ui/DataDialogs/DialogInput.cpp         \
	src/Ui/DataDialogs/DialogProcess.cpp       \
	src/Ui/DataDialogs/DialogProfile.cpp       \
	src/Ui/DialogImport.cpp                    \
	src/Ui/MainDialogs.cpp                     \
	src/Ui/MainWindow.cpp                      \
	src/LEDSpicerUI.cpp

ledspicerui_CPPFLAGS = \
	$(AM_CPPFLAGS)     \
	$(GTKMM30_CFLAGS)  \
	$(TINYXML2_CFLAGS)

ledspicerui_LDADD = \
	$(GTKMM30_LIBS) \
	$(TINYXML2_LIBS)

ledspiceruidatadir=$(pkgdatadir)
ledspiceruidata_DATA = \
	data/main.glade    \
	data/style.css

imagesdir=$(pkgdatadir)/images
images_DATA = \
	$(wildcard data/images/*.png) \
	$(wildcard data/images/*.svg)

#############################
# documentation and samples #
#############################

dist_doc_DATA = \
	README      \
	COPYING

######################
# extra distribution #
######################

DISTCLEANFILES = \
	$(ledspicer_pc_DATA)

EXTRA_DIST =            \
	autogen.sh          \
	src/Defaults.hpp    \
	src/XMLHelper.hpp   \
	src/ConfigFile.hpp  \
	src/InputFile.hpp   \
	src/Ui/Message.hpp  \
	src/Ui/DialogColors.hpp \
	src/Ui/OrdenableFlowBox.hpp \
	src/Ui/OrdenableListBox.hpp \
	src/Ui/Storage/CollectionHandler.hpp \
	src/Ui/Storage/Data.hpp        \
	src/Ui/Storage/NameOnly.hpp      \
	src/Ui/Storage/Element.hpp       \
	src/Ui/Storage/Group.hpp         \
	src/Ui/Storage/Device.hpp        \
	src/Ui/Storage/RestrictorMap.hpp \
	src/Ui/Storage/Restrictor.hpp    \
	src/Ui/Storage/Process.hpp       \
	src/Ui/Storage/InputMapLink.hpp  \
	src/Ui/Storage/InputMap.hpp      \
	src/Ui/Storage/Input.hpp         \
	src/Ui/Storage/Profile.hpp       \
	src/Ui/Storage/BoxButton.hpp     \
	src/Ui/Storage/BoxButtonCollection.hpp \
	src/Ui/DataDialogs/DialogForm.hpp       \
	src/Ui/DataDialogs/DialogElement.hpp    \
	src/Ui/DataDialogs/DialogSelect.hpp     \
	src/Ui/DataDialogs/DialogGroup.hpp  \
	src/Ui/DataDialogs/DialogDevice.hpp \
	src/Ui/DataDialogs/DialogRestrictorMap.hpp \
	src/Ui/DataDialogs/DialogRestrictor.hpp    \
	src/Ui/DataDialogs/DialogInputLinkMaps.hpp \
	src/Ui/DataDialogs/DialogInputMap.hpp      \
	src/Ui/DataDialogs/DialogInput.hpp   \
	src/Ui/DataDialogs/DialogProcess.hpp \
	src/Ui/DataDialogs/DialogProfile.hpp \
	src/Ui/DialogImport.hpp  \
	src/Ui/MainDialogs.hpp   \
	src/Ui/MainWindow.hpp    \
	src/LEDSpicerUI.hpp      \
	AUTHORS             \
	ChangeLog           \
	NEWS