# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2023
# MIT License

add_executable(IntegrationTests
	gbathree.cpp
	round_trip.cpp
)

# check for >=gcc-6 due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67309
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.1)
	target_compile_options(IntegrationTests
		PUBLIC
		-fsingle-precision-constant # issue 544
	)
endif()

target_link_libraries(IntegrationTests catch)
add_test(IntegrationTests IntegrationTests)
