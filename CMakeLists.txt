cmake_minimum_required(VERSION 3.16)
project(LEDSpicerUI VERSION 0.1.0 LANGUAGES CXX)

# Metadata (for glade file substitution and compile definitions)
set(PACKAGE "ledspicerui")
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_TARNAME "ledspicerui")
set(COPYRIGHT "Copyright © 2023 - 2026 Patricio A. Rossi (MeduZa)")
set(DESCRIPTION "Graphical configuration editor for LEDSpicer")
set(MINIMUM_COMPATIBILITY "0.7.0")
set(PACKAGE_URL "https://github.com/meduzapat/LEDSpicerUI")
set(PACKAGE_BUGREPORT "https://github.com/meduzapat/LEDSpicerUI/issues")
set(PACKAGE_DATA_NAME "LEDSpicer")
set(PACKAGE_DATA_VERSION "1.1")

set(APP_ID "org.ledspicer.ui")

# Global compile definitions (for source code)
add_compile_definitions(
	PACKAGE="${PACKAGE}"
	PACKAGE_NAME="${PACKAGE_NAME}"
	PACKAGE_VERSION="${PACKAGE_VERSION}"
	PACKAGE_STRING="${PACKAGE_STRING}"
	PACKAGE_TARNAME="${PACKAGE_TARNAME}"
	PACKAGE_URL="${PACKAGE_URL}"
	PACKAGE_BUGREPORT="${PACKAGE_BUGREPORT}"
	VERSION="${PACKAGE_VERSION}"
	PACKAGE_DATA_NAME="${PACKAGE_DATA_NAME}"
	PACKAGE_DATA_VERSION="${PACKAGE_DATA_VERSION}"
	APP_ID="${APP_ID}"
)

# Build type is selected with -DCMAKE_BUILD_TYPE (Release = -O3 -DNDEBUG), not
# CMakePresets: presets need CMake >= 3.21, while -D works on every supported
# distro (some older ones still ship a pre-3.21 CMake). Default to Release.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM30 REQUIRED gtkmm-3.0>=3.24)
pkg_check_modules(TINYXML2 REQUIRED tinyxml2>=6.0)

# Build-time tools for the embedded GResource bundle (xml-stripblanks needs xmllint).
find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)
find_program(XMLLINT xmllint REQUIRED)

pkg_check_modules(GTEST gtest>=1.8)
if(GTEST_FOUND)
	message(STATUS "Google Test found - tests enabled")
	set(HAVE_GTEST YES)
else()
	message(WARNING "Google Test >= 1.8 not found; tests unavailable")
	set(HAVE_GTEST NO)
endif()

set(STAGE_DIR ${CMAKE_BINARY_DIR}/data)

configure_file(${CMAKE_SOURCE_DIR}/data/main.glade ${STAGE_DIR}/main.glade @ONLY)

file(GLOB_RECURSE RESOURCE_IMAGES
	RELATIVE ${CMAKE_SOURCE_DIR}/data
	CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/data/images/*.png
	${CMAKE_SOURCE_DIR}/data/images/*.svg
)
configure_file(${CMAKE_SOURCE_DIR}/data/style-base.css ${STAGE_DIR}/style-base.css COPYONLY)

set(RESOURCE_STAGED ${STAGE_DIR}/main.glade ${STAGE_DIR}/style-base.css)
set(RESOURCE_IMAGE_ENTRIES "")
foreach(img ${RESOURCE_IMAGES})
	configure_file(${CMAKE_SOURCE_DIR}/data/${img} ${STAGE_DIR}/${img} COPYONLY)
	list(APPEND RESOURCE_STAGED ${STAGE_DIR}/${img})
	string(APPEND RESOURCE_IMAGE_ENTRIES "\t\t<file>${img}</file>\n")
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/data/icons/256/${APP_ID}.png)
	set(WINDOW_ICON ${CMAKE_SOURCE_DIR}/data/icons/256/${APP_ID}.png)
else()
	set(WINDOW_ICON ${CMAKE_SOURCE_DIR}/data/images/LEDSpicerLogo.png)
endif()
configure_file(${WINDOW_ICON} ${STAGE_DIR}/window-icon.png COPYONLY)
list(APPEND RESOURCE_STAGED ${STAGE_DIR}/window-icon.png)
string(APPEND RESOURCE_IMAGE_ENTRIES "\t\t<file>window-icon.png</file>\n")

# Manifest: @RESOURCE_IMAGE_ENTRIES@ holds the generated <file> lines.
configure_file(${CMAKE_SOURCE_DIR}/data/ledspicerui.gresource.xml ${STAGE_DIR}/ledspicerui.gresource.xml @ONLY)

# Compile the staged tree into a linkable source.
set(RESOURCE_SRC ${CMAKE_BINARY_DIR}/resources.cpp)
add_custom_command(
	OUTPUT ${RESOURCE_SRC}
	COMMAND ${GLIB_COMPILE_RESOURCES}
		--target=${RESOURCE_SRC}
		--generate-source
		--sourcedir=${STAGE_DIR}
		${STAGE_DIR}/ledspicerui.gresource.xml
	DEPENDS
		${STAGE_DIR}/ledspicerui.gresource.xml
		${RESOURCE_STAGED}
	COMMENT "Compiling GResource bundle"
	VERBATIM
)

add_executable(ledspicerui
	${RESOURCE_SRC}
	src/Values.cpp
	src/Defaults.cpp
	src/XMLHelper.cpp
	src/config/Settings.cpp
	src/config/SettingsFile.cpp
	src/config/Geometry.cpp
	src/config/ProjectFile.cpp
	src/config/ConfigFile.cpp
	src/config/InputFile.cpp
	src/config/AnimationFile.cpp
	src/config/ProfileFile.cpp
	src/Ui/Message.cpp
	src/Ui/DialogColors.cpp
	src/Ui/DialogProject.cpp
	src/Ui/DialogPrompt.cpp
	src/Ui/DialogSettings.cpp
	src/Ui/SortableFlowBox.cpp
	src/Ui/SortableListBox.cpp
	src/Ui/Layout/StripRenderer.cpp
	src/Ui/Layout/LayoutTester.cpp
	src/Ui/Layout/LayoutElement.cpp
	src/Ui/Layout/LayoutBoard.cpp
	src/Ui/Layout/Layout.cpp
	src/Ui/Storage/CollectionHandler.cpp
	src/Ui/Storage/Data.cpp
	src/Ui/Storage/Parent.cpp
	src/Ui/Storage/DirNode.cpp
	src/Ui/Storage/Selection.cpp
	src/Ui/Storage/Link.cpp
	src/Ui/Storage/DirectoryEntry.cpp
	src/Ui/Storage/Element.cpp
	src/Ui/Storage/Group.cpp
	src/Ui/Storage/Device.cpp
	src/Ui/Storage/RestrictorMap.cpp
	src/Ui/Storage/Restrictor.cpp
	src/Ui/Storage/Process.cpp
	src/Ui/Storage/InputMapLink.cpp
	src/Ui/Storage/InputMap.cpp
	src/Ui/Storage/InputSource.cpp
	src/Ui/Storage/Input.cpp
	src/Ui/Storage/Profile.cpp
	src/Ui/Storage/Transition.cpp
	src/Ui/Storage/Actor.cpp
	src/Ui/Storage/Animation.cpp
	src/Ui/Storage/BoxButton.cpp
	src/Ui/Storage/BoxButtonCollection.cpp
	src/Ui/DataDialogs/DialogForm.cpp
	src/Ui/DataDialogs/DialogFormHost.cpp
	src/Ui/DataDialogs/DirectoryAware.cpp
	src/Ui/DataDialogs/DialogLinkEditor.cpp
	src/Ui/DataDialogs/DialogSelect.cpp
	src/Ui/DataDialogs/DialogElement.cpp
	src/Ui/DataDialogs/DialogGroup.cpp
	src/Ui/DataDialogs/DialogDevice.cpp
	src/Ui/DataDialogs/DialogRestrictorMap.cpp
	src/Ui/DataDialogs/DialogRestrictor.cpp
	src/Ui/DataDialogs/DialogInputSource.cpp
	src/Ui/DataDialogs/DialogInputLinkMaps.cpp
	src/Ui/DataDialogs/DialogInputMap.cpp
	src/Ui/DataDialogs/DialogInput.cpp
	src/Ui/DataDialogs/DialogProcess.cpp
	src/Ui/DataDialogs/DialogProfile.cpp
	src/Ui/DataDialogs/DialogTransition.cpp
	src/Ui/DataDialogs/DialogActor.cpp
	src/Ui/DataDialogs/DialogAnimation.cpp
	src/Ui/DirectoryNavigator.cpp
	src/Ui/InputDirectoryNavigator.cpp
	src/Ui/AnimationDirectoryNavigator.cpp
	src/Ui/ProfileDirectoryNavigator.cpp
	src/Ui/DialogImport.cpp
	src/Ui/ThemeManager.cpp
	src/Ui/StatusBar.cpp
	src/Ui/MainWindow.cpp
	src/LEDSpicerUI.cpp
)

target_include_directories(ledspicerui PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/Ui
	${GTKMM30_INCLUDE_DIRS}
	${TINYXML2_INCLUDE_DIRS}
)

target_compile_options(ledspicerui PRIVATE
	-Wall
	-Wextra
	-fexceptions
)

target_link_libraries(ledspicerui PRIVATE
	${GTKMM30_LIBRARIES}
	${TINYXML2_LIBRARIES}
)

target_compile_definitions(ledspicerui PRIVATE
	PACKAGE_CONF_DIR="${CMAKE_SYSCONFDIR}/"
)

# Core UI assets ship inside the GResource bundle, so only the binary installs.
install(TARGETS ledspicerui DESTINATION bin)

# Optional freedesktop integration, off by default.
option(INSTALL_DESKTOP_ENTRY "Install a freedesktop .desktop launcher and icon" OFF)
if(INSTALL_DESKTOP_ENTRY)
	configure_file(${CMAKE_SOURCE_DIR}/data/${APP_ID}.desktop ${STAGE_DIR}/${APP_ID}.desktop @ONLY)
	install(FILES ${STAGE_DIR}/${APP_ID}.desktop DESTINATION share/applications)
	install(FILES ${CMAKE_SOURCE_DIR}/data/icons/scalable/${APP_ID}.svg DESTINATION share/icons/hicolor/scalable/apps)
	install(FILES ${CMAKE_SOURCE_DIR}/data/icons/256/${APP_ID}.png      DESTINATION share/icons/hicolor/256x256/apps)
	install(FILES ${CMAKE_SOURCE_DIR}/data/icons/48/${APP_ID}.png       DESTINATION share/icons/hicolor/48x48/apps)
endif()

if(HAVE_GTEST)
	add_subdirectory(tests)
endif()

message(STATUS "--------- Build Environment -----------")
message(STATUS "Program:       ${PROJECT_NAME}")
message(STATUS "Version:       ${PROJECT_VERSION}")
message(STATUS "Build Type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "Compatibility: ${MINIMUM_COMPATIBILITY}")
message(STATUS "C++ Compiler:  ${CMAKE_CXX_COMPILER}")
message(STATUS "GTKmm:         ${GTKMM30_VERSION}")
message(STATUS "TinyXML2:      ${TINYXML2_VERSION}")
message(STATUS "Prefix:        ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Desktop Entry: ${INSTALL_DESKTOP_ENTRY}")
message(STATUS "Tests Enabled: ${HAVE_GTEST}")
