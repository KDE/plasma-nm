add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kded\")

include_directories(${CMAKE_SOURCE_DIR}/libs/editor/widgets)

if (WITH_MODEMMANAGER_SUPPORT)
    set(kded_networkmanagement_SRCS
        ../libs/debug.cpp
        bluetoothmonitor.cpp
        connectivitymonitor.cpp
        notification.cpp
        modemmonitor.cpp
        monitor.cpp
        passworddialog.cpp
        pindialog.cpp
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
        connectivitymonitor.cpp
        notification.cpp
        monitor.cpp
        passworddialog.cpp
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
    KF5::ConfigCore
    KF5::DBusAddons
    KF5::I18n
    KF5::IconThemes
    KF5::Notifications
    KF5::Service
    KF5::Solid
    KF5::Wallet
    KF5::WindowSystem
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(kded_networkmanagement KF5::ModemManagerQt)
endif()

install(TARGETS kded_networkmanagement DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)

install(FILES networkmanagement.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
