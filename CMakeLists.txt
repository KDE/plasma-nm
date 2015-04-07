project(plasma-networkmanagement)

cmake_minimum_required(VERSION 2.8)

find_package(KDE4 "4.9.0" REQUIRED)
find_package(NetworkManager "0.9.8.4" REQUIRED)

include(FindPkgConfig)
include(KDE4Defaults)

set(PLASMA_NM_VERSION 0.9.3.6)
set(PLASMA_NM_STRING_VERSION "${PLASMA_NM_VERSION}")

pkg_check_modules(NETWORKMANAGERQT REQUIRED NetworkManagerQt)
if (NOT NETWORKMANAGERQT_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
elseif(${NETWORKMANAGERQT_VERSION} VERSION_LESS 0.9.8.2)
    message(FATAL_ERROR "ERROR: NetworkManagerQt required version is 0.9.8.2. http://projects.kde.org/libnm-qt")
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

add_definitions(
    -DWITH_MODEMMANAGER_SUPPORT=${WITH_MODEMMANAGER_SUPPORT}
    ${NETWORKMANAGERQT_CFLAGS_OTHER})

include_directories(${CMAKE_CURRENT_BINARY_DIR} # for config.h
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/editor/
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${NETWORKMANAGERQT_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

if (MODEMMANAGERQT_FOUND)
    include_directories(${MODEMMANAGERQT_INCLUDE_DIRS})
endif()

add_subdirectory(applet)
add_subdirectory(editor)
add_subdirectory(kded)
add_subdirectory(libs)
add_subdirectory(vpn)
add_subdirectory(settings)

macro_display_feature_log()
