cmake_minimum_required(VERSION 3.16)
project(LEDSpicerUI VERSION 0.0.10 LANGUAGES CXX)

# Metadata (for glade file substitution and compile definitions)
set(PACKAGE "ledspicerui")
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_TARNAME "ledspicerui")
set(COPYRIGHT "Copyright © 2023 - 2025 Patricio A. Rossi (MeduZa)")
set(MINIMUM_COMPATIBILITY "0.7.0")
set(PACKAGE_URL "https://github.com/meduzapat/LEDSpicerUI")
set(PACKAGE_BUGREPORT "https://github.com/meduzapat/LEDSpicerUI/issues")
set(PACKAGE_DATA_NAME "LEDSpicer")
set(PACKAGE_DATA_VERSION "1.1")

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
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM30 REQUIRED gtkmm-3.0>=3.24)
pkg_check_modules(TINYXML2 REQUIRED tinyxml2>=6.0)

pkg_check_modules(GTEST gtest>=1.8)
if(GTEST_FOUND)
	message(STATUS "Google Test found - tests enabled")
	set(HAVE_GTEST YES)
else()
	message(WARNING "Google Test >= 1.8 not found; tests unavailable")
	set(HAVE_GTEST NO)
endif()

add_executable(ledspicerui
	src/Defaults.cpp
	src/XMLHelper.cpp
	src/config/ProjectFile.cpp
	src/config/SettingsFile.cpp
	src/config/ConfigFile.cpp
	src/config/InputFile.cpp
	src/Ui/Message.cpp
	src/Ui/DialogColors.cpp
	src/Ui/DialogProject.cpp
	src/Ui/DialogSettings.cpp
	src/Ui/OrdenableFlowBox.cpp
	src/Ui/OrdenableListBox.cpp
	src/Ui/Storage/CollectionHandler.cpp
	src/Ui/Storage/Data.cpp
	src/Ui/Storage/DirNode.cpp
	src/Ui/Storage/FileData.cpp
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
	src/Ui/Storage/BoxButton.cpp
	src/Ui/Storage/BoxButtonCollection.cpp
	src/Ui/DataDialogs/DialogForm.cpp
	src/Ui/DataDialogs/DialogFormHost.cpp
	src/Ui/DataDialogs/DialogFileForm.cpp
	src/Ui/DataDialogs/DialogSelect.cpp
	src/Ui/DataDialogs/DialogElement.cpp
	src/Ui/DataDialogs/DialogGroup.cpp
	src/Ui/DataDialogs/DialogDevice.cpp
	src/Ui/DataDialogs/DialogRestrictorMap.cpp
	src/Ui/DataDialogs/DialogRestrictor.cpp
	src/Ui/DataDialogs/DialogDirectory.cpp
	src/Ui/DataDialogs/DialogInputSource.cpp
	src/Ui/DataDialogs/DialogInputLinkMaps.cpp
	src/Ui/DataDialogs/DialogInputMap.cpp
	src/Ui/DataDialogs/DialogInput.cpp
	src/Ui/DataDialogs/DialogProcess.cpp
	src/Ui/DataDialogs/DialogProfile.cpp
	src/Ui/DirectoryNavigator.cpp
	src/Ui/InputDirectoryNavigator.cpp
	src/Ui/DialogImport.cpp
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
	PACKAGE_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/"
)

configure_file(
	${CMAKE_SOURCE_DIR}/data/main.glade.in
	${CMAKE_BINARY_DIR}/data/main.glade
	@ONLY
)

set(DATA_FILES
	${CMAKE_BINARY_DIR}/data/main.glade
	data/style.css
)

file(GLOB IMAGE_FILES "data/images/*.png" "data/images/*.svg")

install(TARGETS ledspicerui DESTINATION bin)
install(FILES ${DATA_FILES} DESTINATION share/${PROJECT_NAME})
install(FILES ${IMAGE_FILES} DESTINATION share/${PROJECT_NAME}/images)

if(HAVE_GTEST)
	add_subdirectory(tests)
endif()

message(STATUS "--------- Build Environment -----------")
message(STATUS "Program:       ${PROJECT_NAME}")
message(STATUS "Version:       ${PROJECT_VERSION}")
message(STATUS "Compatibility: ${MINIMUM_COMPATIBILITY}")
message(STATUS "C++ Compiler:  ${CMAKE_CXX_COMPILER}")
message(STATUS "GTKmm:         ${GTKMM30_VERSION}")
message(STATUS "TinyXML2:      ${TINYXML2_VERSION}")
message(STATUS "Prefix:        ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Tests Enabled: ${HAVE_GTEST}")
