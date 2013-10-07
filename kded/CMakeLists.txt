if (MODEMMANAGERQT_FOUND)
    set(kded_networkmanagement_SRCS
        notification.cpp
        passworddialog.cpp
        secretagent.cpp
        service.cpp
#         // TODO: port to MM1
#         bluetoothmonitor.cpp
#         modemmonitor.cpp
#         pindialog.cpp
    )
    kde4_add_ui_files(kded_networkmanagement_SRCS
#         pinwidget.ui
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
    kde4_add_ui_files(kded_networkmanagement_SRCS
        passworddialog.ui
    )
endif()

kde4_add_plugin(kded_networkmanagement ${kded_networkmanagement_SRCS})

target_link_libraries(kded_networkmanagement
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    plasmanetworkmanagement-internal
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_SOLID_LIBS}
)

install(TARGETS kded_networkmanagement DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES networkmanagement.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES networkmanagement.notifyrc DESTINATION ${DATA_INSTALL_DIR}/networkmanagement)
