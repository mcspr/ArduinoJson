# ArduinoJson - https://arduinojson.org
# Copyright © 2014-2026, Benoit BLANCHON
# MIT License

if(MSVC_VERSION LESS 1910)
	return()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5)
	return()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
	return()
endif()

add_executable(Cpp17Tests
	string_view.cpp
)

set_property(TARGET Cpp17Tests PROPERTY CXX_STANDARD 17)
set_property(TARGET Cpp17Tests PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(Cpp17Tests catch)
add_test(Cpp17 Cpp17Tests)
