cmake_minimum_required(VERSION 3.10)
project(FormalAutomation LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(GTest)
find_package(Threads)

include_directories (${GTEST_INCLUDE_DIRS})

add_subdirectory(lib ${PROJECT_SOURCE_DIR}/library)

add_executable(Auto.out main.cpp)
add_executable(AutoTest test/main_test.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(Auto.out library)
target_link_libraries(AutoTest library)

target_include_directories(Auto.out PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(AutoTest PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(AutoTest ${GTEST_LIBRARIES} Threads::Threads)