set(openvpn_SRCS
    openvpn.cpp
    openvpnwidget.cpp
    openvpnauth.cpp
    openvpnadvancedwidget.cpp
)

qt5_wrap_ui(openvpn_SRCS openvpn.ui openvpnauth.ui openvpnadvanced.ui)

plasma_add_plugin(plasmanetworkmanagement_openvpnui ${openvpn_SRCS})

target_link_libraries(plasmanetworkmanagement_openvpnui plasmanetworkmanagement-internal ${NETWORKMANAGERQT_LDFLAGS} ${KDE4_KIO_LIBS})

install(TARGETS plasmanetworkmanagement_openvpnui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_openvpnui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
