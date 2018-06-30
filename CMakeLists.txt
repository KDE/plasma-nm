cmake_minimum_required(VERSION 3.0)

project(plasma-networkmanagement)

set(PROJECT_VERSION "5.13.80")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.9.0")
set(KF5_MIN_VERSION "5.42.0")

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Network
    Quick
    Widgets
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED
    I18n
    WindowSystem
    Service
    Completion
    WidgetsAddons
    KIO
    CoreAddons
    Wallet
    ConfigWidgets
    IconThemes
    Solid
    DBusAddons
    Notifications
    Plasma
    Declarative
    NetworkManagerQt
    KDELibs4Support
)

find_package(KF5ModemManagerQt 5.14.0)
set_package_properties(KF5ModemManagerQt PROPERTIES
                       TYPE OPTIONAL)

# Required only for getting information about NetworkManager version in CMake
find_package(NetworkManager 1.0.0)
set_package_properties(NetworkManager PROPERTIES
                       TYPE REQUIRED)

find_package(MobileBroadbandProviderInfo)
set_package_properties(MobileBroadbandProviderInfo PROPERTIES
                       DESCRIPTION "Database of mobile broadband service providers"
                       URL "http://live.gnome.org/NetworkManager/MobileBroadband/ServiceProviders"
                       TYPE OPTIONAL)

find_package(Qca-qt5 2.1.0)
set_package_properties(Qca-qt5 PROPERTIES DESCRIPTION "Support for encryption"
                       URL "http://download.kde.org/stable/qca-qt5/"
                       TYPE REQUIRED)

if (DISABLE_MODEMMANAGER_SUPPORT)
    message(STATUS "Disabling ModemManager support")
    set(WITH_MODEMMANAGER_SUPPORT 0)
else()
    if (KF5ModemManagerQt_FOUND)
        message(STATUS "Enabling ModemManager support")
        set(WITH_MODEMMANAGER_SUPPORT 1)
    else()
        message(STATUS "ModemManager or ModemManagerQt not found")
        set(WITH_MODEMMANAGER_SUPPORT 0)
    endif()
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)

add_definitions(-DWITH_MODEMMANAGER_SUPPORT=${WITH_MODEMMANAGER_SUPPORT})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/editor/)

add_subdirectory(applet)
add_subdirectory(kded)
add_subdirectory(kcm)
add_subdirectory(libs)
add_subdirectory(vpn)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
