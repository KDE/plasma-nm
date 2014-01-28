set(vpnc_SRCS
    vpnc.cpp
    vpncwidget.cpp
    vpncadvancedwidget.cpp
    vpncauth.cpp
)

kde4_add_ui_files(vpnc_SRCS vpnc.ui vpncadvanced.ui vpncauth.ui)

kde4_add_plugin(plasmanetworkmanagement_vpncui ${vpnc_SRCS})

target_link_libraries(plasmanetworkmanagement_vpncui plasmanetworkmanagement-editor ${NETWORKMANAGERQT_LDFLAGS} ${KDE4_KIO_LIBS})

install(TARGETS plasmanetworkmanagement_vpncui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_vpncui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
