project(plasma-nm)

cmake_minimum_required(VERSION 2.6)

find_package(KDE4 REQUIRED)
find_package(NetworkManager "0.9.8" REQUIRED)
find_package(KDE4 "4.9.0" REQUIRED)

include(FindPkgConfig)
include(KDE4Defaults)

pkg_check_modules(MODEMMANAGERQT REQUIRED ModemManagerQt)
if (NOT MODEMMANAGERQT_FOUND)
    message(FATAL_ERROR "ERROR: ModemManagerQt not found. http://projects.kde.org/libmm-qt")
endif (NOT MODEMMANAGERQT_FOUND)

pkg_check_modules(NETWORKMANAGERQT REQUIRED NetworkManagerQt)
if (NOT NETWORKMANAGERQT_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
endif (NOT NETWORKMANAGERQT_FOUND)

set(KDE_NETWORK_VERSION "0.1.0")
configure_file(config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})
macro_optional_find_package(MobileBroadbandProviderInfo)
macro_log_feature(MOBILEBROADBANDPROVIDERINFO_FOUND "mobile-broadband-provider-info" "Database of mobile broadband service providers" "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders" FALSE "" "Needed for Mobile Connection Wizard support in Broadband Modem Management")

pkg_check_modules(QTNETWORK REQUIRED QtNetwork)
pkg_check_modules(NETWORKMANAGER REQUIRED NetworkManager)

add_definitions(
    ${QT_DEFINITIONS}
    ${KDE_DEFINITIONS}
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/lib
                    ${QTNETWORK_INCLUDE_DIRS}
                    ${MODEMMANAGERQT_INCLUDE_DIRS}
                    ${NETWORKMANAGERQT_INCLUDE_DIRS}
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

add_subdirectory(lib)
add_subdirectory(applet)
add_subdirectory(declarative-plugins)
add_subdirectory(editor)
add_subdirectory(vpn)
add_subdirectory(kded)
add_subdirectory(kcm)
