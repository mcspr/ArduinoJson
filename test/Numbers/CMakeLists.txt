# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2023
# MIT License

add_executable(NumbersTests
	isFloat.cpp
	isInteger.cpp
	parseFloat.cpp
	parseDouble.cpp
	parseInteger32.cpp
	parseInteger64.cpp
)

target_link_libraries(NumbersTests catch)
add_test(Numbers NumbersTests)
