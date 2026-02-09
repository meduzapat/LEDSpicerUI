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

# Define a macro to create test executables
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

# Common source files used by multiple tests
set(COMMON_SRCS
	${CMAKE_SOURCE_DIR}/src/Defaults.cpp
)

# Define each test using the macro
add_test_executable(DefaultsTest
	"DefaultsTest.cpp"
	"${COMMON_SRCS}"
	""
)

add_test_executable(DataTest
	"Ui/Storage/DataTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Storage/Data.cpp"
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

add_test_executable(ConfigFileTest
	"ConfigFileTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButton.cpp;${CMAKE_SOURCE_DIR}/src/Ui/Storage/BoxButtonCollection.cpp;${CMAKE_SOURCE_DIR}/src/ConfigFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

add_test_executable(InputFileTest
	"InputFileTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/Ui/Message.cpp;${CMAKE_SOURCE_DIR}/src/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/ProjectFile.cpp;${CMAKE_SOURCE_DIR}/src/InputFile.cpp"
	"${TINYXML2_LIBRARIES}"
)

# Set colored output for all tests
set_tests_properties(DefaultsTest DataTest MessageTest XMLHelperTest ConfigFileTest InputFileTest
	PROPERTIES ENVIRONMENT "GTEST_COLOR=1"
)