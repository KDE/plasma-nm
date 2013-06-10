set(pptp_SRCS
   pptp.cpp
   pptpwidget.cpp
   pptpauth.cpp
   )

kde4_add_ui_files(pptp_SRCS pptpprop.ui pptpadvanced.ui pptpauth.ui)

kde4_add_plugin(plasmanm_pptpui ${pptp_SRCS})

target_link_libraries(plasmanm_pptpui NetworkManagerQt ${KDE4_KIO_LIBS} plasmanm-internal)

install(TARGETS plasmanm_pptpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES plasmanm_pptpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
