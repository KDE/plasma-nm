cmake_minimum_required(VERSION 3.16)

project(plasma-networkmanagement)

set(PROJECT_VERSION "5.27.12")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.102.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(ECMFindQmlModule)
include(KDEGitCommitHooks)
include(ECMQtDeclareLoggingCategory)

include(FeatureSummary)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Gui
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
    NetworkManagerQt
    Notifications
    Plasma
    Service
    Solid
    Wallet
    WidgetsAddons
    WindowSystem
    KCMUtils
    ModemManagerQt
)

ecm_find_qmlmodule(org.kde.prison 1.0)

find_package(KF5Kirigami2 ${KF5_MIN_VERSION} CONFIG)
set_package_properties(KF5Kirigami2 PROPERTIES
    DESCRIPTION "A QtQuick based components set"
    PURPOSE "Required at runtime by the KCMs"
    TYPE RUNTIME
)

find_package(Qca-qt${QT_MAJOR_VERSION} 2.1.0)
set_package_properties(Qca-qt${QT_MAJOR_VERSION} PROPERTIES DESCRIPTION "Support for encryption"
                       URL "https://download.kde.org/stable/qca-qt5/"
                       TYPE REQUIRED)

find_package(KF5Prison ${KF5_MIN_VERSION})
set_package_properties(KF5Prison PROPERTIES DESCRIPTION "Prison library"
                       URL "https://commits.kde.org/prison"
                       TYPE RUNTIME
                       PURPOSE "Needed to create mobile barcodes for WiFi networks"
                      )

find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENCONNECT IMPORTED_TARGET openconnect>=3.99)
pkg_check_modules(NETWORKMANAGER IMPORTED_TARGET libnm>1.4.0 REQUIRED)
pkg_check_modules(MOBILEBROADBANDPROVIDERINFO mobile-broadband-provider-info)
pkg_get_variable(BROADBANDPROVIDER_DATABASE mobile-broadband-provider-info database)

add_definitions(
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050f00
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055800
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
)


add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)

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
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ecm_qt_install_logging_categories(
        EXPORT PLASMANM
        FILE plasma-nm.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
