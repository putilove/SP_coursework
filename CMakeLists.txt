cmake_minimum_required (VERSION 3.8)

project(course_work)

add_executable(scan "ScanPorts.cpp")

find_package(Threads REQUIRED)
target_link_libraries(scan PRIVATE Threads::Threads)