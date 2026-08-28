# ArduinoJson - https://arduinojson.org
# Copyright © 2014-2026, Benoit BLANCHON
# MIT License

include(GNUInstallDirs)

add_library(ArduinoJson INTERFACE)
target_include_directories(ArduinoJson
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(ArduinoJson
	INTERFACE
		ARDUINOJSON_DEBUG=$<CONFIG:Debug>
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}ArduinoJson.h"
		"${CMAKE_CURRENT_SOURCE_DIR}ArduinoJson.hpp"
	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY
		"${CMAKE_CURRENT_SOURCE_DIR}/ArduinoJson"
	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)
