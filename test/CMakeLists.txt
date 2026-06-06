# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2023
# MIT License

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ASAN_OPTIONS handle_abort=1)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
	add_compile_options(
		-pedantic
		-Wall
		-Wcast-align
		-Wcast-qual
		-Wconversion
		-Wctor-dtor-privacy
		-Wdisabled-optimization
		-Werror
		-Wextra
		-Wformat=2
		-Winit-self
		-Wmissing-include-dirs
		-Wold-style-cast
		-Woverloaded-virtual
		-Wparentheses
		-Wredundant-decls
		-Wreturn-type
		-Wshadow
		-Wsign-conversion
		-Wsign-promo
		-Wstrict-aliasing
		-Wundef
	)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
	   CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0)
		add_compile_options(
			-fsanitize=address,undefined
			-fsanitize-trap=all
		)
		add_link_options(
			-fsanitize=address,undefined
			-fsanitize-trap=all
		)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	add_compile_options(
		-Wstrict-null-sentinel
	)

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.5)
		add_compile_options(-Wlogical-op) # the flag exists in 4.4 but is buggy
	endif()

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.6)
		add_compile_options(-Wnoexcept)
	endif()

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 AND
	   CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
		# avoid false positive with GCC 4.7
		add_compile_options(-Wno-maybe-uninitialized)
	endif()

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5.0)
		# avoid false positive with < GCC 5.0
		# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=62232
		add_compile_options(-Wnon-virtual-dtor)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(
		-Wc++11-compat
		-Wdeprecated-register
		-Wnon-virtual-dtor
	)
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_compile_options(
		/W4 # Set warning level
		/WX # Treats all compiler warnings as errors.
	)
endif()

add_subdirectory(DynamicJsonBuffer)
add_subdirectory(IntegrationTests)
add_subdirectory(JsonArray)
add_subdirectory(JsonBuffer)
add_subdirectory(JsonObject)
add_subdirectory(JsonVariant)
add_subdirectory(JsonWriter)
add_subdirectory(Misc)
add_subdirectory(Numbers)
add_subdirectory(StaticJsonBuffer)
