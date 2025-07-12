cmake_minimum_required(VERSION 3.22)
project(CalcGTest LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

add_library(CalcLib
        ICalculator.h
        SimpleCalculator.h SimpleCalculator.cpp
        IHistory.h
        InMemoryHistory.h InMemoryHistory.cpp
)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
	Calculator_test
	CalculatorTest.cpp
)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  message(STATUS "Coverage enabled")
  target_compile_options(Calculator_test PRIVATE --coverage -fprofile-arcs -ftest-coverage)
  target_link_options(Calculator_test PRIVATE --coverage -fprofile-arcs)
  
  find_program(GCOVR_PATH gcovr)
  
  if(GCOVR_PATH)
    message(STATUS "Gcov found")
    add_custom_target(coverage
      COMMAND ${GCOVR_PATH} 
	  --exclude ".*/tests/.*" 
	  --html-details -o ${CMAKE_BINARY_DIR}/coverage_report/index.html
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS Calculator_test
    )
  else()
    message(WARNING "Gcov not found")
  endif()
  add_test(NAME UnitTest COMMAND Calculator_test)
else()
  message(WARNING "Coverage disabled")
endif()

target_link_libraries (
	Calculator_test
	CalcLib
	gtest
	gmock
)