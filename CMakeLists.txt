cmake_minimum_required(VERSION 3.0.0)
project(DLL VERSION 0.1.0)

file(GLOB SRC "src/*.cpp")
set(SRC ${SRC} main.cpp)

add_executable(DLL ${SRC})

target_include_directories(DLL PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(DLL libpdcurses)

#Static start
set_target_properties(DLL PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(DLL PROPERTIES LINK_SEARCH_END_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set_target_properties(DLL PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
