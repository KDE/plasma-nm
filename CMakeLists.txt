project(plasma-networkmanagement)

cmake_minimum_required(VERSION 2.8)

find_package(KDE4 "4.9.0" REQUIRED)
find_package(NetworkManager "0.9.8.4" REQUIRED)

include(FindPkgConfig)
include(KDE4Defaults)

set(PLASMA_NM_VERSION "0.9.3.4-git")

pkg_check_modules(NETWORKMANAGERQT REQUIRED NetworkManagerQt)
if (NOT NETWORKMANAGERQT_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
elseif(${NETWORKMANAGERQT_VERSION} VERSION_LESS 0.9.8.1)
    message(FATAL_ERROR "ERROR: NetworkManagerQt required version is 0.9.8.1. http://projects.kde.org/libnm-qt")
endif()

if (DISABLE_MODEMMANAGER_SUPPORT)
    message(STATUS "Disabling ModemManager support")
    set(WITH_MODEMMANAGER_SUPPORT 0)
else()
    pkg_check_modules(MODEMMANAGERQT ModemManagerQt)
    if (MODEMMANAGERQT_FOUND)
        message(STATUS "Enabling ModemManager support")
        set(WITH_MODEMMANAGER_SUPPORT 1)

    else()
        message(STATUS "ModemManagerQt not found")
        set(WITH_MODEMMANAGER_SUPPORT 0)
    endif()
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

if (MODEMMANAGERQT_FOUND)
    macro_optional_find_package(MobileBroadbandProviderInfo)
    macro_log_feature(MOBILEBROADBANDPROVIDERINFO_FOUND "mobile-broadband-provider-info" "Database of mobile broadband service providers" "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders" FALSE "" "Needed for Mobile Connection Wizard support in Broadband Modem Management")
endif()
pkg_check_modules(NETWORKMANAGER REQUIRED NetworkManager)
pkg_check_modules(QTNETWORK REQUIRED QtNetwork)

add_definitions(
    -DWITH_MODEMMANAGER_SUPPORT=${WITH_MODEMMANAGER_SUPPORT}
    ${QT_DEFINITIONS}
    ${KDE_DEFINITIONS}
    ${NETWORKMANAGERQT_CFLAGS_OTHER}
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/editor/
                    ${QTNETWORK_INCLUDE_DIRS}
                    if (MODEMMANAGERQT_FOUND)
                        ${MODEMMANAGER_INCLUDE_DIRS}
                        ${MODEMMANAGERQT_INCLUDE_DIRS}
                    endif()
                    ${NETWORKMANAGERQT_INCLUDE_DIRS}
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

add_subdirectory(applet)
add_subdirectory(declarative-plugins)
add_subdirectory(editor)
add_subdirectory(kded)
add_subdirectory(libs)
add_subdirectory(vpn)
add_subdirectory(settings)

macro_display_feature_log()
