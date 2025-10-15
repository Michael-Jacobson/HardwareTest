cmake_minimum_required(VERSION 3.8)

project (linuxhardwaretest VERSION 1.0.0 DESCRIPTION "Ultratec Hardware Test App")

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(NETWORKMANAGER REQUIRED libnm)

# Find source files
file(GLOB SOURCES src/*.c)

# Exlcude XX.c
#list(FILTER SOURCES EXCLUDE REGEX "XX.c")

# Include header files
include_directories("Include")

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC "-no-pie")
target_link_options(${PROJECT_NAME} PUBLIC "-no-pie")
target_link_options(${PROJECT_NAME} PUBLIC "-L${CMAKE_SOURCE_DIR}/Lib")

target_include_directories(${PROJECT_NAME} PUBLIC ${GLIB_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${NETWORKMANAGER_INCLUDE_DIRS})

# link libpthread
target_link_libraries(${PROJECT_NAME} pthread m asound ${GLIB_LIBRARIES} ${NETWORKMANAGER_LIBRARIES})

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install library headers
file(GLOB HEADERS Include/*.h)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# build testcases
#ENABLE_TESTING()
#ADD_SUBDIRECTORY( tests )
