project(plasma-networkmanagement)

cmake_minimum_required(VERSION 2.6)

find_package(KDE4 "4.9.0" REQUIRED)
find_package(NetworkManager "0.9.8.4" REQUIRED)

include(FindPkgConfig)
include(KDE4Defaults)

set(PLASMA_NM_VERSION "0.9.3.3")

pkg_check_modules(NETWORKMANAGERQT REQUIRED NetworkManagerQt)
if (NOT NETWORKMANAGERQT_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
elseif(${NETWORKMANAGER_VERSION} VERSION_EQUAL 0.9.9.0 AND ${NETWORKMANAGERQT_VERSION} VERSION_LESS 0.9.9.1)
    message(FATAL_ERROR "ERROR: NetworkManagerQt required version is 0.9.9.1. http://projects.kde.org/libnm-qt")
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

        if (${MODEMMANAGERQT_VERSION} VERSION_GREATER 0.7.990)
            set(MODEMMANAGERQT_ONE 1)
        endif()
    else()
        message(STATUS "ModemManagerQt not found")
        set(WITH_MODEMMANAGER_SUPPORT 0)
    endif()
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

macro_optional_find_package(MobileBroadbandProviderInfo)
macro_log_feature(MOBILEBROADBANDPROVIDERINFO_FOUND "mobile-broadband-provider-info" "Database of mobile broadband service providers" "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders" FALSE "" "Needed for Mobile Connection Wizard support in Broadband Modem Management")
pkg_check_modules(QTNETWORK REQUIRED QtNetwork)
pkg_check_modules(NETWORKMANAGER REQUIRED NetworkManager)

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
                    ${CMAKE_CURRENT_SOURCE_DIR}/lib
                    ${CMAKE_CURRENT_SOURCE_DIR}/lib/editor
                    ${QTNETWORK_INCLUDE_DIRS}
                    if (MODEMMANAGERQT_FOUND)
                        ${MODEMMANAGER_INCLUDE_DIRS}
                        ${MODEMMANAGERQT_INCLUDE_DIRS}
                    endif()
                    ${NETWORKMANAGERQT_INCLUDE_DIRS}
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

add_subdirectory(lib)
add_subdirectory(applet)
add_subdirectory(declarative-plugins)
add_subdirectory(editor)
add_subdirectory(vpn)
add_subdirectory(kded)
add_subdirectory(settings)

macro_display_feature_log()
