cmake_minimum_required(VERSION 3.20)
project(Pointes_of_No_Return)

set(CMAKE_CXX_STANDARD 20)

include_directories("/opt/homebrew/include")
link_directories("/opt/homebrew/lib")

add_executable(Pointes_of_No_Return main.cpp)

target_link_libraries(Pointes_of_No_Return sfml-graphics sfml-window sfml-system sfml-audio)