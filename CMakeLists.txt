cmake_minimum_required (VERSION 3.8)

project(course_work)

add_executable(main "main.cpp" "functions.h" "functions.cpp")
add_executable(serv "serv.cpp")
add_executable(client "client.cpp")


find_package(Threads REQUIRED)
target_link_libraries(main PRIVATE Threads::Threads)
