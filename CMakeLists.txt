cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -march=native -mtune=native")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    message("Building for debug")
else()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")
    message("Building for release, all librarys need to be compiled for release!")
endif ()


project(SopraGameLogic VERSION 0.0.1 DESCRIPTION "GameLogic for SoPra")

set(SOURCES
        ${CMAKE_SOURCE_DIR}/src/GameModel.cpp
        ${CMAKE_SOURCE_DIR}/src/Action.cpp
        ${CMAKE_SOURCE_DIR}/src/GameController.cpp
        ${CMAKE_SOURCE_DIR}/src/Interference.cpp
        ${CMAKE_SOURCE_DIR}/src/conversions.cpp)
set(LIBS SopraMessages)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Installation via make install
include(GNUInstallDirs)
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER
        "src/GameModel.h;src/Action.h;src/GameController.h;src/Interference.h;src/conversions.h;src/SharedPtrSerialization.h")
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SopraGameLogic)

add_subdirectory(Tests)
