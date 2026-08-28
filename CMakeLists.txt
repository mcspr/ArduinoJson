# ArduinoJson - https://arduinojson.org
# Copyright © 2014-2026, Benoit BLANCHON
# MIT License

cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ArduinoJson VERSION 5.13.6)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	include(${CMAKE_CURRENT_LIST_DIR}/CompileOptions.cmake)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third-party/catch)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fuzzing)
endif()
