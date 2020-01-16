cmake_minimum_required(VERSION 3.0)

project(plasma-networkmanagement)

set(PROJECT_VERSION "5.17.90")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.12.0")
set(KF5_MIN_VERSION "5.66.0")

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(ECMQMLModules)

include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Network
    Quick
    QuickWidgets
    Widgets
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED
    ConfigWidgets
    Completion
    CoreAddons
    Declarative
    DBusAddons
    KIO
    I18n
    IconThemes
    NetworkManagerQt
    Notifications
    Plasma
    Service
    Solid
    Wallet
    WidgetsAddons
    WindowSystem
)

find_package(KF5ModemManagerQt ${KF5_MIN_VERSION})
set_package_properties(KF5ModemManagerQt PROPERTIES
                       TYPE OPTIONAL)

ecm_find_qmlmodule(org.kde.prison 1.0)

if (BUILD_MOBILE)
    find_package(KF5Kirigami2 ${KF5_MIN_VERSION} CONFIG)
    set_package_properties(KF5Kirigami2 PROPERTIES
        DESCRIPTION "A QtQuick based components set"
        PURPOSE "Required at runtime by the mobile KCMs"
        TYPE RUNTIME
    )
endif()

# Required only for getting information about NetworkManager version in CMake
find_package(NetworkManager 1.4.0)
set_package_properties(NetworkManager PROPERTIES
                       TYPE REQUIRED)

find_package(MobileBroadbandProviderInfo)
set_package_properties(MobileBroadbandProviderInfo PROPERTIES
                       DESCRIPTION "Database of mobile broadband service providers"
                       URL "https://wiki.gnome.org/action/show/Projects/NetworkManager/MobileBroadband/ServiceProviders"
                       TYPE OPTIONAL)

find_package(Qca-qt5 2.1.0)
set_package_properties(Qca-qt5 PROPERTIES DESCRIPTION "Support for encryption"
                       URL "https://download.kde.org/stable/qca-qt5/"
                       TYPE REQUIRED)

find_package(KF5Prison ${KF5_MIN_VERSION})
set_package_properties(KF5Prison PROPERTIES DESCRIPTION "Prison library"
                       URL "https://commits.kde.org/prison"
                       TYPE RUNTIME
                       PURPOSE "Needed to create mobile barcodes for WiFi networks"
                      )

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

if (BUILD_MOBILE)
    add_subdirectory(mobile)
endif()

# Enable unit testing
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
