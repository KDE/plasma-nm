if (WITH_MODEMMANAGER_SUPPORT)
    set(kded_networkmanagement_SRCS
        notification.cpp
        passworddialog.cpp
        secretagent.cpp
        service.cpp
        bluetoothmonitor.cpp
        modemmonitor.cpp
        pindialog.cpp
    )
    qt5_wrap_ui(kded_networkmanagement_SRCS
        pinwidget.ui
        passworddialog.ui

    )
else()
    set(kded_networkmanagement_SRCS
        notification.cpp
        passworddialog.cpp
        secretagent.cpp
        service.cpp
        bluetoothmonitor.cpp
    )
    qt5_wrap_ui(kded_networkmanagement_SRCS
        passworddialog.ui
    )
endif()

plasma_add_plugin(kded_networkmanagement ${kded_networkmanagement_SRCS})

target_link_libraries(kded_networkmanagement
    plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LIBRARY}
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
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
    KF5::KDE4Support
)

install(TARGETS kded_networkmanagement DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES networkmanagement.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES networkmanagement.notifyrc DESTINATION ${DATA_INSTALL_DIR}/networkmanagement)
