cmake_minimum_required(VERSION 3.27)
project("LuaBirali")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_executable(LuaBirali 
 src/LuaBirali.cpp
 src/Scanner.cpp
 src/Token.cpp
 src/Expression.cpp 
 src/Parser.cpp
 src/Interpreter.cpp 
 src/Environment.cpp
 src/Resolver.cpp
 )

 include_directories(include)

target_precompile_headers ( LuaBirali
    PRIVATE
    <iostream>
    <optional>
    <concepts>
    <cstdio>
    <fstream>
    <functional>
    <iostream>
    <map>
    <mutex>
    <optional>
    <queue>
    <span>
    <string>
    <thread>
    <type_traits>
    <unordered_map>
    <variant>
    <vector>
    <variant>
)

add_custom_command(
    TARGET LuaBirali POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/out/build/${CMAKE_PRESET_NAME}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
)
