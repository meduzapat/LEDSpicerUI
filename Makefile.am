#################
# Automake file #
#################

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =                             \
	-I$(srcdir)/src                       \
	-DPACKAGE_DATA_DIR=\"$(pkgdatadir)/\" \
	-Wl,--no-as-needed                    \
	-fexceptions

bin_PROGRAMS = ledspicerui

ledspicerui_SOURCES =   \
	src/Message.cpp     \
	src/XMLHelper.cpp   \
	src/ConfigFile.cpp  \
	src/Ui/OrdenableFlowBox.cpp \
	src/Ui/Forms/PinHandler.cpp \
	src/Ui/Forms/CollectionHandler.cpp \
	src/Ui/Forms/Form.cpp              \
	src/Ui/Forms/NameOnly.cpp          \
	src/Ui/Forms/Element.cpp           \
	src/Ui/Forms/Group.cpp             \
	src/Ui/Forms/Device.cpp     \
	src/Ui/Forms/Restrictor.cpp \
	src/Ui/Forms/BoxButton.cpp  \
	src/Ui/Forms/BoxButtonCollection.cpp \
	src/Ui/DialogColors.cpp  \
	src/Ui/DialogForm.cpp    \
	src/Ui/DialogSelectElements.cpp \
	src/Ui/DialogGroup.cpp   \
	src/Ui/DialogDevice.cpp  \
	src/Ui/DialogRestrictor.cpp \
	src/Ui/DialogElement.cpp \
	src/Ui/MainWindow.cpp    \
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
	src/Message.hpp     \
	src/XMLHelper.hpp   \
	src/ConfigFile.hpp  \
	src/Ui/OrdenableFlowBox.hpp \
	src/Ui/Forms/PinHandler.hpp \
	src/Ui/Forms/CollectionHandler.hpp \
	src/Ui/Forms/Form.hpp              \
	src/Ui/Forms/NameOnly.hpp          \
	src/Ui/Forms/Element.hpp           \
	src/Ui/Forms/Group.hpp             \
	src/Ui/Forms/Device.hpp     \
	src/Ui/Forms/Restrictor.hpp \
	src/Ui/Forms/BoxButton.hpp  \
	src/Ui/Forms/BoxButtonCollection.hpp \
	src/Ui/DialogColors.hpp  \
	src/Ui/DialogForm.hpp    \
	src/Ui/DialogSelectElements.hpp \
	src/Ui/DialogGroup.hpp   \
	src/Ui/DialogDevice.hpp  \
	src/Ui/DialogRestrictor.hpp \
	src/Ui/DialogElement.hpp \
	src/Ui/MainWindow.hpp    \
	src/LEDSpicerUI.hpp \
	AUTHORS             \
	ChangeLog           \
	NEWS