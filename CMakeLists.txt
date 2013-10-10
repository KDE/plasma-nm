project(plasma-networkmanagement)

set(DISABLE_MODEMMANAGER_SUPPORT 1)

cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)

#find_package(KDE4 "4.9.0" REQUIRED)
find_package(NetworkManager "0.9.8" REQUIRED)

include(FindPkgConfig)
#include(KDE4Defaults)

################# set KDE specific information #################

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

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
#find_package(Qt5X11Extras REQUIRED NO_MODULE)
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
find_package(KF5 MODULE REQUIRED COMPONENTS CMake Compiler InstallDirs
    KIdleTime ItemModels KWidgetsAddons KWindowSystem KCodecs KArchive KCoreAddons Solid ThreadWeaver
    KConfig KAuth KJS KWallet KDBusAddons Sonnet
    KI18n KGuiAddons KService KConfigWidgets ItemViews KNotifications KIconThemes KStyle KCompletion KJobWidgets KTextWidgets XmlGui KCrash
    KBookmarks KUnitConversion KDE4Attic KCMUtils Kross)

find_package(Plasma REQUIRED NO_MODULE)
find_package(KDeclarative REQUIRED NO_MODULE)

find_package(KInit)

# needed for set_package_properties macro
find_package(KDELibs4 REQUIRED NO_MODULE)
find_package(KDE4Support REQUIRED NO_MODULE)
find_package(Qt5Transitional MODULE)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

if (DISABLE_MODEMMANAGER_SUPPORT)
    message(STATUS "Disabling ModemManager support")
    set(WITH_MODEMMANAGER_SUPPORT 0)
    set(MODEMMANAGERQT_FOUND false)
    set(MODEMMANAGERQT_LIBRARY "")
else()
    pkg_check_modules(MODEMMANAGERQT ModemManagerQt)
    if (MODEMMANAGERQT_FOUND)
        message(STATUS "Enabling ModemManager support")
        set(WITH_MODEMMANAGER_SUPPORT 1)
        set(MODEMMANAGERQT_LIBRARY ModemManagerQt)
        if (${MODEMMANAGERQT_VERSION} VERSION_GREATER 0.7.990)
            message(STATUS "Building against ModemManager 1")
            set(MODEMMANAGERQT_ONE 1)
        endif()
    else()
        message(STATUS "ModemManagerQt not found")
        set(WITH_MODEMMANAGER_SUPPORT 0)
    endif()
endif()

pkg_check_modules(NETWORKMANAGERQT REQUIRED NetworkManagerQt)
if (NOT NETWORKMANAGERQT_FOUND)
    message(FATAL_ERROR "ERROR: NetworkManagerQt not found. http://projects.kde.org/libnm-qt")
endif (NOT NETWORKMANAGERQT_FOUND)

set(KDE_NETWORK_VERSION "0.9.3.1")
set(PLASMA_NM_VERSION "0.9.3.1")


configure_file(config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})


find_package(MobileBroadbandProviderInfo)
set_package_properties(MobileBroadbandProviderInfo PROPERTIES
                       DESCRIPTION "Database of mobile broadband service providers"
                       URL "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders"
                       TYPE OPTIONAL
                      )


#macro_optional_find_package(MobileBroadbandProviderInfo)
#macro_log_feature(MOBILEBROADBANDPROVIDERINFO_FOUND "mobile-broadband-provider-info" "Database of mobile broadband service providers" "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders" FALSE "" "Needed for Mobile Connection Wizard support in Broadband Modem Management")

#pkg_check_modules(QTNETWORK REQUIRED QtNetwork)
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
                    ${MODEMMANAGER_INCLUDE_DIRS}
                    ${NETWORKMANAGERQT_INCLUDE_DIRS}
                    ${NETWORKMANAGER_INCLUDE_DIRS}
                    ${KDE4_INCLUDES})

if (MODEMMANAGERQT_FOUND)
    include_directories(${MODEMMANAGERQT_INCLUDE_DIRS})
endif()

add_subdirectory(lib)
add_subdirectory(applet)
add_subdirectory(declarative-plugins)
#add_subdirectory(editor)
add_subdirectory(vpn)
add_subdirectory(kded)
#add_subdirectory(kcm)
add_subdirectory(settings)
