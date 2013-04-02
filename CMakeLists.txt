project(plasma-nm)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(NetworkManager "0.9.8" REQUIRED)

include(FindPkgConfig)

pkg_check_modules(QTNETWORKMANAGER REQUIRED QtNetworkManager)
if (NOT QTNETWORKMANAGER_FOUND)
    message(FATAL_ERROR "ERROR: QtNetworkManager not found. http://projects.kde.org/libnm-qt")
endif (NOT QTNETWORKMANAGER_FOUND)

pkg_check_modules(QTNETWORK REQUIRED QtNetwork)
pkg_check_modules(NETWORKMANAGER REQUIRED NetworkManager)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${QTNETWORK_INCLUDE_DIRS}
                    ${QTNETWORKMANAGER_INCLUDE_DIRS}
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

add_subdirectory(applet)
add_subdirectory(declarative-plugins)
add_subdirectory(editor)
add_subdirectory(vpn)
add_subdirectory(lib)
