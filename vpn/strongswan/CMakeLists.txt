set(strongswan_SRCS
   strongswan.cpp
   strongswanwidget.cpp
   strongswanauth.cpp
   )

kde4_add_ui_files(strongswan_SRCS strongswanprop.ui strongswanauth.ui)

kde4_add_plugin(plasmanetworkmanagement_strongswanui ${strongswan_SRCS})

target_link_libraries(plasmanetworkmanagement_strongswanui NetworkManagerQt ${KDE4_KIO_LIBS} plasmanetworkmanagement-internal)

install(TARGETS plasmanetworkmanagement_strongswanui  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES plasmanetworkmanagement_strongswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
