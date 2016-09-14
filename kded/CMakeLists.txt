add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kded\")

include_directories(${CMAKE_SOURCE_DIR}/libs/editor/widgets)

if (WITH_MODEMMANAGER_SUPPORT)
    set(kded_networkmanagement_SRCS
        ../libs/debug.cpp
        bluetoothmonitor.cpp
        notification.cpp
        modemmonitor.cpp
        monitor.cpp
        passworddialog.cpp
        pindialog.cpp
        portalmonitor.cpp
        secretagent.cpp
        service.cpp
    )
    ki18n_wrap_ui(kded_networkmanagement_SRCS
        pinwidget.ui
        passworddialog.ui
    )
else()
    set(kded_networkmanagement_SRCS
        ../libs/debug.cpp
        bluetoothmonitor.cpp
        notification.cpp
        monitor.cpp
        passworddialog.cpp
        portalmonitor.cpp
        secretagent.cpp
        service.cpp
    )
    ki18n_wrap_ui(kded_networkmanagement_SRCS
        passworddialog.ui
    )
endif()

add_library(kded_networkmanagement MODULE ${kded_networkmanagement_SRCS})
set_target_properties(kded_networkmanagement PROPERTIES OUTPUT_NAME networkmanagement)
kcoreaddons_desktop_to_json(kded_networkmanagement networkmanagement.desktop)

target_link_libraries(kded_networkmanagement
    plasmanm_internal
    plasmanm_editor
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::NetworkManagerQt
    KF5::Service
    KF5::Solid
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::DBusAddons
    KF5::Notifications
    KF5::Wallet
    KF5::WindowSystem
    KF5::KIOWidgets
    KF5::CoreAddons
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(kded_networkmanagement KF5::ModemManagerQt)
endif()

install(TARGETS kded_networkmanagement DESTINATION ${PLUGIN_INSTALL_DIR}/kf5/kded)

install(FILES networkmanagement.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
