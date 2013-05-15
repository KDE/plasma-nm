set(openvpn_SRCS
    openvpn.cpp
    openvpnwidget.cpp
    openvpnauth.cpp
    openvpnadvancedwidget.cpp
)

kde4_add_ui_files(openvpn_SRCS openvpn.ui openvpnauth.ui openvpnadvanced.ui)

kde4_add_plugin(plasmanm_openvpnui ${openvpn_SRCS})

target_link_libraries(plasmanm_openvpnui plasmanm-internal NetworkManagerQt ${KDE4_KIO_LIBS})

install(TARGETS plasmanm_openvpnui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm_openvpnui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
