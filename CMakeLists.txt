project(plasma-networkmanagement)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.2.0")

set(PROJECT_VERSION "5.1.90")
set(PROJECT_VERSION_MAJOR 5)
set(PLASMANM_VERSION_STRING ${PROJECT_VERSION} )

################# set KDE specific information #################

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

include(ECMPackageConfigHelpers)
include(ECMOptionalAddSubdirectory)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Network
    Quick
    Widgets
)

find_package(KF5 REQUIRED
    I18n WindowSystem Service Completion WidgetsAddons KIO CoreAddons Wallet ItemViews XmlGui
    ConfigWidgets IconThemes Solid DBusAddons Notifications Plasma Declarative Init KDELibs4Support NetworkManagerQt)

find_package(KF5ModemManagerQt)
set_package_properties(KF5ModemManagerQt PROPERTIES
                       TYPE OPTIONAL)

find_package(NetworkManager)
set_package_properties(NetworkManager PROPERTIES
                       TYPE REQUIRED)

find_package(ModemManager)
set_package_properties(ModemManager PROPERTIES
                       TYPE OPTIONAL)

find_package(MobileBroadbandProviderInfo)
set_package_properties(MobileBroadbandProviderInfo PROPERTIES
                       DESCRIPTION "Database of mobile broadband service providers"
                       URL "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders"
                       TYPE OPTIONAL)

if (DISABLE_MODEMMANAGER_SUPPORT)
    message(STATUS "Disabling ModemManager support")
    set(WITH_MODEMMANAGER_SUPPORT 0)
else()
    if (KF5ModemManagerQt_FOUND AND MODEMMANAGER_FOUND)
        message(STATUS "Enabling ModemManager support")
        set(WITH_MODEMMANAGER_SUPPORT 1)
    else()
        message(STATUS "ModemManager or ModemManagerQt not found")
        set(WITH_MODEMMANAGER_SUPPORT 0)
    endif()
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

add_definitions(-DWITH_MODEMMANAGER_SUPPORT=${WITH_MODEMMANAGER_SUPPORT})

include_directories(${CMAKE_CURRENT_BINARY_DIR} # for config.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/editor/
                    ${NETWORKMANAGER_INCLUDE_DIRS})

if (WITH_MODEMMANAGER_SUPPORT)
    include_directories(${MODEMMANAGER_INCLUDE_DIRS})
endif()

add_subdirectory(applet)
add_subdirectory(editor)
add_subdirectory(kded)
add_subdirectory(libs)
add_subdirectory(vpn)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
