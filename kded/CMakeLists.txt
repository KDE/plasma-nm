set(kded_plasmanm_SRCS
   interfacenotification.cpp
   passworddialog.cpp
   secretagent.cpp
   service.cpp
   pindialog.cpp
   modemmonitor.cpp
   bluetoothmonitor.cpp
)

kde4_add_ui_files(kded_plasmanm_SRCS
    passworddialog.ui
    pinwidget.ui
)

kde4_add_plugin(kded_plasmanm ${kded_plasmanm_SRCS})

target_link_libraries(kded_plasmanm
    NetworkManagerQt
    ModemManagerQt
    plasmanm-internal
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_SOLID_LIBS}
)

install(TARGETS kded_plasmanm DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES networkmanagement.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES plasma-nm.notifyrc DESTINATION ${DATA_INSTALL_DIR}/plasma-networkmanagement)
