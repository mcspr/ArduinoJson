# ArduinoJson - https://arduinojson.org
# Copyright © 2014-2026, Benoit BLANCHON
# MIT License

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_libraries(ArduinoJson)
link_libraries(catch)

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
