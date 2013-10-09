if (MODEMMANAGERQT_FOUND)
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
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    plasmanetworkmanagement-internal
    
    ${KDE4_KDEUI_LIBS}
    KF5::Solid
)

install(TARGETS kded_networkmanagement DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES networkmanagement.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES networkmanagement.notifyrc DESTINATION ${DATA_INSTALL_DIR}/networkmanagement)
