# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2023
# MIT License

add_executable(JsonBufferTests
	nested.cpp
	nestingLimit.cpp
	parse.cpp
	parseBom.cpp
	parseBomEnabled.cpp
	parseValidate.cpp
	parseComments.cpp
	parseCommentsEnabled.cpp
	parseArray.cpp
	parseArrayComments.cpp
	parseObject.cpp
	parseObjectComments.cpp
	parseKeyValue.cpp
)

target_link_libraries(JsonBufferTests catch)
add_test(JsonBuffer JsonBufferTests)
