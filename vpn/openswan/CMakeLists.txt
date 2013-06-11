set(openswan_SRCS
    openswan.cpp
    openswanwidget.cpp
    openswanauth.cpp
)

kde4_add_ui_files(openswan_SRCS openswan.ui openswanauth.ui)

kde4_add_plugin(plasmanm_openswanui ${openswan_SRCS})

target_link_libraries(plasmanm_openswanui plasmanm-internal NetworkManagerQt ${KDE4_KIO_LIBS})

install(TARGETS plasmanm_openswanui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm_openswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
