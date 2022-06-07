cmake_minimum_required (VERSION 3.8)

project(course_work)

add_executable(ncat "main.cpp" "functions.h" "functions.cpp")


find_package(Threads REQUIRED)
target_link_libraries(ncat PRIVATE Threads::Threads)
