# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2019
# MIT License

set(CMAKE_CXX_STANDARD 11)

add_library(catch
	catch.hpp
	catch.cpp
)

target_include_directories(catch
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(catch
	PUBLIC
		CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
)
