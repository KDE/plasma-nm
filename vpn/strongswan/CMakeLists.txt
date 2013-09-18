set(strongswan_SRCS
   strongswan.cpp
   strongswanwidget.cpp
   strongswanauth.cpp
   )

kde4_add_ui_files(strongswan_SRCS strongswanprop.ui strongswanauth.ui)

kde4_add_plugin(plasmanm_strongswanui ${strongswan_SRCS})

target_link_libraries(plasmanm_strongswanui NetworkManagerQt ${KDE4_KIO_LIBS} plasmanm-internal)

install(TARGETS plasmanm_strongswanui  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES plasmanm_strongswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
