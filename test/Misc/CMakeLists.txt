# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2023
# MIT License

add_executable(MiscTests
	empty.cpp
	FloatParts.cpp
	std_stream.cpp
	std_string.cpp
	std_string_flash_string_operators.cpp
	StringBuilder.cpp
	StringTraits.cpp
	TypeTraits.cpp
	unsigned_char.cpp
	unsigned_char_flash_string_operators.cpp
	const_unsigned_char.cpp
	version.cpp
)

target_include_directories(MiscTests PRIVATE shim)
target_link_libraries(MiscTests catch)
add_test(Misc MiscTests)
