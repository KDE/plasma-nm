project(plasma-networkmanagement)

cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)
set(QT_MIN_VERSION "5.2.0")

set(PLASMA_NM_VERSION_MAJOR 1)
set(PLASMA_NM_VERSION_MINOR 0)
set(PLASMA_NM_VERSION_PATCH 0)
set(PLASMA_NM_VERSION ${PLASMA_NM_VERSION_MAJOR}.${PLASMA_NM_VERSION_MINOR}.${PLASMA_NM_VERSION_PATCH} )

# With CMake older than version 3.1 pkg_check_modules ignores CMAKE_PREFIX_PATH
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
include(FindPkgConfig)

################# set KDE specific information #################

find_package(ECM 0.0.8 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(WriteBasicConfigVersionFile)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(ECMOptionalAddSubdirectory)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core DBus Quick Widgets Designer Network Test)

# Load CMake, Compiler and InstallDirs settings from KF5 and the following are already somewhat "done" tier1/tier2 libs from kdelibs:
find_package(KF5 CONFIG REQUIRED
    I18n WindowSystem Service Completion WidgetsAddons KIO CoreAddons Wallet ItemViews XmlGui
    ConfigWidgets IconThemes Solid DBusAddons Notifications Plasma Declarative Init KDE4Support)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

pkg_check_modules(NETWORKMANAGER REQUIRED NetworkManager)
pkg_check_modules(NETWORKMANAGERQT5 REQUIRED NetworkManagerQt5)

if (NOT NETWORKMANAGERQT5_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
endif (NOT NETWORKMANAGERQT5_FOUND)

if (DISABLE_MODEMMANAGER_SUPPORT)
    message(STATUS "Disabling ModemManager support")
    set(WITH_MODEMMANAGER_SUPPORT 0)
    set(MODEMMANAGERQT5_FOUND false)
    set(MODEMMANAGERQT5_LIBRARY "")
else()
    pkg_check_modules(MODEMMANAGERQT5 ModemManagerQt5)
    if (MODEMMANAGERQT5_FOUND)
        message(STATUS "Enabling ModemManager support")
        set(WITH_MODEMMANAGER_SUPPORT 1)
        set(MODEMMANAGERQT5_LIBRARY ModemManagerQt5)
    else()
        message(STATUS "ModemManagerQt5 not found")
        set(WITH_MODEMMANAGER_SUPPORT 0)
    endif()
endif()

# TODO
if (MODEMMANAGERQT5_FOUND)
    find_package(MobileBroadbandProviderInfo)
    set_package_properties(MobileBroadbandProviderInfo PROPERTIES
                           DESCRIPTION "Database of mobile broadband service providers"
                           URL "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders"
                           TYPE OPTIONAL)
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

add_definitions(
    -DWITH_MODEMMANAGER_SUPPORT=${WITH_MODEMMANAGER_SUPPORT}
    ${QT_DEFINITIONS}
    ${KDE_DEFINITIONS}
    ${NETWORKMANAGERQT5_CFLAGS_OTHER}
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/editor/
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${NETWORKMANAGERQT5_INCLUDE_DIRS}
                    ${Qt5Designer_INCLUDE_DIRS})

if (MODEMMANAGERQT5_FOUND)
include_directories(
      ${MODEMMANAGER_INCLUDE_DIRS}
      ${MODEMMANAGERQT5_INCLUDE_DIRS}
)
endif()

add_subdirectory(applet)
add_subdirectory(editor)
add_subdirectory(kded)
add_subdirectory(libs)
add_subdirectory(settings)
add_subdirectory(vpn)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

