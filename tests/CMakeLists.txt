cmake_minimum_required(VERSION 3.16)
project(LEDSpicerUI_Tests LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# These include paths are added to the compile command for all test source files.
include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/Ui
	${CMAKE_SOURCE_DIR}/tests/Ui
	${GTKMM30_INCLUDE_DIRS}
	${TINYXML2_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIRS}
)

# Compiler flags for warnings and exceptions, applied to all test targets.
add_compile_options(-Wall -Wextra -fexceptions)

# Test executables and their settings.
add_executable(DefaultsTest
	DefaultsTest.cpp
	${CMAKE_SOURCE_DIR}/src/Defaults.cpp
)
target_compile_definitions(DefaultsTest PRIVATE
	PACKAGE_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/tests/"
)
target_link_libraries(DefaultsTest PRIVATE ${GTKMM30_LIBRARIES} ${GTEST_LIBRARIES})
add_test(NAME DefaultsTest COMMAND DefaultsTest)

add_executable(DataTest
	Ui/Storage/DataTest.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Storage/Data.cpp
	${CMAKE_SOURCE_DIR}/src/Defaults.cpp
)
target_compile_definitions(DataTest PRIVATE
	PACKAGE_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/tests/"
)
target_link_libraries(DataTest PRIVATE ${GTKMM30_LIBRARIES} ${GTEST_LIBRARIES})
add_test(NAME DataTest COMMAND DataTest)

add_executable(MessageTest
	Ui/MessageTest.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp
	${CMAKE_SOURCE_DIR}/src/Defaults.cpp
)
target_compile_definitions(MessageTest PRIVATE
	PACKAGE_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/tests/"
)
target_link_libraries(MessageTest PRIVATE ${GTKMM30_LIBRARIES} ${GTEST_LIBRARIES})
add_test(NAME MessageTest COMMAND MessageTest)

add_executable(XMLHelperTest
	XMLHelperTest.cpp
	${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp
	${CMAKE_SOURCE_DIR}/src/Defaults.cpp
	${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp
)
target_compile_definitions(XMLHelperTest PRIVATE
	PACKAGE_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/tests/"
)
target_link_libraries(XMLHelperTest PRIVATE ${GTKMM30_LIBRARIES} ${TINYXML2_LIBRARIES} ${GTEST_LIBRARIES})
add_test(NAME XMLHelperTest COMMAND XMLHelperTest)

# Sets colored output for Google Test runs via ctest.
set_tests_properties(DefaultsTest DataTest MessageTest XMLHelperTest PROPERTIES ENVIRONMENT "GTEST_COLOR=1")
