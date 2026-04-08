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
	${CMAKE_SOURCE_DIR}/src/Values.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Data.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButton.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButtonCollection.cpp
)

# Storage sources including FileNode — needed by FileNode subclasses.
set(STORAGE_FILENODE_SRCS
	${STORAGE_SRCS}
	${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirNode.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/FileNode.cpp
)

set(STORAGE_BASE_SRCS
	${COMMON_SRCS}
	${STORAGE_SRCS}
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp
)

set(STORAGE_INPUT_SRCS
	${STORAGE_BASE_SRCS}
	${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Revertible.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirNode.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/FileNode.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Input.cpp
)

set(STORAGE_PROFILE_SRCS
	${STORAGE_BASE_SRCS}
	${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirNode.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/FileNode.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Profile.cpp
)

add_test_executable(DefaultsTest
	"DefaultsTest.cpp"
	"${COMMON_SRCS}"
	""
)

add_test_executable(ValuesTest
	"ValuesTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Values.cpp"
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
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Revertible.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp"
	""
)

add_test_executable(BoxButtonTest
	"Ui/Storage/BoxButtonTest.cpp"
	"${COMMON_SRCS};${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp"
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
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirNode.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/DirectoryEntry.cpp"
	""
)

add_test_executable(FileNodeTest
	"Ui/Storage/FileNodeTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${STORAGE_FILENODE_SRCS}"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(ConfigFileTest
	"config/ConfigFileTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${STORAGE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/CollectionHandler.cpp;${CMAKE_SOURCE_DIR}/src/config/ConfigFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(ProjectFileTest
	"config/ProjectFileTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/config/InputFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(InputFileTest
	"config/InputFileTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/config/InputFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(SettingsFileTest
	"config/SettingsFileTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/config/SettingsFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(LeafDataTest
	"Ui/Storage/LeafDataTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Link.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/RestrictorMap.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/InputMap.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/InputMapLink.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Process.cpp"
	""
)

add_test_executable(LinkTest
	"Ui/Storage/LinkTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Link.cpp"
	""
)

add_test_executable(GroupTest
	"Ui/Storage/GroupTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Group.cpp"
	""
)

add_test_executable(DeviceTest
	"Ui/Storage/DeviceTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Revertible.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Device.cpp"
	""
)

add_test_executable(RestrictorTest
	"Ui/Storage/RestrictorTest.cpp"
	"${STORAGE_BASE_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Parent.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Revertible.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/Restrictor.cpp"
	""
)

add_test_executable(InputTest
	"Ui/Storage/InputTest.cpp"
	"${STORAGE_INPUT_SRCS}"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(ProfileTest
	"Ui/Storage/ProfileTest.cpp"
	"${STORAGE_PROFILE_SRCS}"
	"${TINYXML2_LIBRARIES}"
)

set_tests_properties(
	DefaultsTest DataTest ParentTest RevertibleTest MessageTest XMLHelperTest
	LeafDataTest LinkTest GroupTest DeviceTest RestrictorTest
	DirectoryEntryTest FileNodeTest InputTest
	ConfigFileTest InputFileTest ProjectFileTest SettingsFileTest
	PROPERTIES ENVIRONMENT "GTEST_COLOR=1"
)
