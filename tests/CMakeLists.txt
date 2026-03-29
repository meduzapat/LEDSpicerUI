cmake_minimum_required(VERSION 3.16)
project(LEDSpicerUI_Tests LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/Ui
	${CMAKE_SOURCE_DIR}/tests/Ui
	${GTKMM30_INCLUDE_DIRS}
	${TINYXML2_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIRS}
)

add_compile_options(-Wall -Wextra -fexceptions)

macro(add_test_executable TEST_NAME TEST_SRC EXTRA_SRCS EXTRA_LIBS)
	add_executable(${TEST_NAME}
		${TEST_SRC}
		${EXTRA_SRCS}
	)
	target_compile_definitions(${TEST_NAME} PRIVATE
		PACKAGE_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/tests/"
	)
	target_link_libraries(${TEST_NAME} PRIVATE
		${GTKMM30_LIBRARIES}
		${GTEST_LIBRARIES}
		gtest_main
		${EXTRA_LIBS}
	)
	add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# Common source files used by multiple tests.
set(COMMON_SRCS
	${CMAKE_SOURCE_DIR}/src/Defaults.cpp
)

# Storage sources needed by anything that includes DirectoryEntry.hpp.
set(STORAGE_SRCS
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Data.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButton.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButtonCollection.cpp
)
#	${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirNode.cpp
#	${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirectoryEntry.cpp

# Storage sources including Revertible — needed by FileData subclasses.
set(STORAGE_REVERTIBLE_SRCS
	${STORAGE_SRCS}
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Revertible.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/FileData.cpp
)

add_test_executable(DefaultsTest
	"DefaultsTest.cpp"
	"${COMMON_SRCS}"
	""
)

add_test_executable(DataTest
	"Ui/Storage/DataTest.cpp"
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp"
	""
)

add_test_executable(ParentTest
	"Ui/Storage/ParentTest.cpp"
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp"
	""
)

add_test_executable(RevertibleTest
	"Ui/Storage/RevertibleTest.cpp"
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Revertible.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp"
	""
)

add_test_executable(MessageTest
	"Ui/MessageTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp"
	""
)

add_test_executable(XMLHelperTest
	"XMLHelperTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(DirectoryEntryTest
	"Ui/Storage/DirectoryEntryTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${STORAGE_SRCS}"
	""
)

add_test_executable(FileDataTest
	"Ui/Storage/FileDataTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${STORAGE_REVERTIBLE_SRCS}"
	""
)

add_test_executable(ConfigFileTest
	"config/ConfigFileTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButton.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButtonCollection.cpp;${CMAKE_SOURCE_DIR}/src/config/ConfigFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(ProjectFileTest
	"config/ProjectFileTest.cpp"
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/config/ProjectFile.cpp;${CMAKE_SOURCE_DIR}/src/config/InputFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(InputFileTest
	"config/InputFileTest.cpp"
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/config/ProjectFile.cpp;${CMAKE_SOURCE_DIR}/src/config/InputFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(SettingsFileTest
	"config/SettingsFileTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/config/SettingsFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

set_tests_properties(
	DefaultsTest DataTest MessageTest XMLHelperTest
	DirectoryEntryTest FileDataTest
	ConfigFileTest InputFileTest ProjectFileTest SettingsFileTest
	PROPERTIES ENVIRONMENT "GTEST_COLOR=1"
)
