set(vpnc_SRCS
    vpnc.cpp
    vpncwidget.cpp
    vpncadvancedwidget.cpp
    vpncauth.cpp
)

kde4_add_ui_files(vpnc_SRCS vpnc.ui vpncadvanced.ui vpncauth.ui)

kde4_add_plugin(plasmanm_vpncui ${vpnc_SRCS})

target_link_libraries(plasmanm_vpncui plasmanm-internal QtNetworkManager ${KDE4_KIO_LIBS})

install(TARGETS plasmanm_vpncui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm_vpncui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
