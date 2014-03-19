project(plasma-networkmanagement)

set(DISABLE_MODEMMANAGER_SUPPORT 1)

cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)

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

set(QT_MIN_VERSION "5.2.0")
find_package(Qt5Core REQUIRED NO_MODULE)
find_package(Qt5DBus REQUIRED NO_MODULE)
find_package(Qt5Quick REQUIRED NO_MODULE)
find_package(Qt5Widgets REQUIRED NO_MODULE)
find_package(Qt5Designer REQUIRED NO_MODULE)
find_package(Qt5Network REQUIRED NO_MODULE)
find_package(Qt5Test REQUIRED NO_MODULE)

include_directories(${Qt5Widgets_INCLUDE_DIRS}
                    ${Qt5DBus_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Designer_INCLUDE_DIRS}
                    ${Qt5Quick_INCLUDE_DIRS}
                    ${Qt5Network_INCLUDE_DIRS}
                    ${Qt5Test_INCLUDE_DIRS}
                    )

# Load CMake, Compiler and InstallDirs settings from KF5 and the following are already somewhat "done" tier1/tier2 libs from kdelibs:
find_package(KF5 CONFIG REQUIRED
    IdleTime ItemModels WidgetsAddons WindowSystem Codecs Archive CoreAddons Solid ThreadWeaver
    Config Auth JS Wallet DBusAddons Sonnet
    I18n GuiAddons Service ConfigWidgets ItemViews Notifications IconThemes Completion JobWidgets TextWidgets XmlGui Crash
    Bookmarks UnitConversion KCMUtils Plasma)

find_package(KF5Plasma REQUIRED NO_MODULE)
find_package(KF5Declarative REQUIRED NO_MODULE)

find_package(KF5Init)

# needed for set_package_properties macro
find_package(KF5KDE4Support REQUIRED NO_MODULE)
find_package(Qt5Transitional MODULE)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

pkg_check_modules(NETWORKMANAGERQT5 REQUIRED NetworkManagerQt5)
# if (NOT NETWORKMANAGERQT_FOUND)
#     message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
# elseif(${NETWORKMANAGER_VERSION} VERSION_EQUAL 0.9.9.0 AND ${NETWORKMANAGERQT_VERSION} VERSION_LESS 0.9.9.1)
#     message(FATAL_ERROR "ERROR: NetworkManagerQt required version is 0.9.9.1. http://projects.kde.org/libnm-qt")
# elseif(${NETWORKMANAGERQT_VERSION} VERSION_LESS 0.9.8.1)
#     message(FATAL_ERROR "ERROR: NetworkManagerQt required version is 0.9.8.1. http://projects.kde.org/libnm-qt")
# endif()

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

pkg_check_modules(NETWORKMANAGERQT5 REQUIRED NetworkManagerQt5)
if (NOT NETWORKMANAGERQT5_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
endif (NOT NETWORKMANAGERQT5_FOUND)

set(KDE_NETWORK_VERSION "1.0.0")
set(PLASMA_NM_VERSION "1.0.0")

configure_file(config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})


find_package(MobileBroadbandProviderInfo)
set_package_properties(MobileBroadbandProviderInfo PROPERTIES
                       DESCRIPTION "Database of mobile broadband service providers"
                       URL "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders"
                       TYPE OPTIONAL
                      )

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

if (MODEMMANAGERQT5_FOUND)
    macro_optional_find_package(MobileBroadbandProviderInfo)
    macro_log_feature(MOBILEBROADBANDPROVIDERINFO_FOUND "mobile-broadband-provider-info" "Database of mobile broadband service providers" "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders" FALSE "" "Needed for Mobile Connection Wizard support in Broadband Modem Management")
endif()
pkg_check_modules(NETWORKMANAGER REQUIRED NetworkManager)

add_definitions(
    -DWITH_MODEMMANAGER_SUPPORT=${WITH_MODEMMANAGER_SUPPORT}
    ${QT_DEFINITIONS}
    ${KDE_DEFINITIONS}
    ${NETWORKMANAGERQT5_CFLAGS_OTHER}
)

if (DISABLE_MODEMMANAGER_SUPPORT)
    remove_definitions( -DWITH_MODEMMANAGERQT=1 )
endif()

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/editor/
                    if (MODEMMANAGERQT5_FOUND)
                        ${MODEMMANAGER_INCLUDE_DIRS}
                        ${MODEMMANAGERQT5_INCLUDE_DIRS}
                    endif()
                    ${NETWORKMANAGERQT5_INCLUDE_DIRS}
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

add_subdirectory(applet)
add_subdirectory(declarative-plugins)
add_subdirectory(editor)
add_subdirectory(kded)
add_subdirectory(libs)
add_subdirectory(vpn)
add_subdirectory(settings)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

