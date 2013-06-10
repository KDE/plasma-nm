set(l2tp_SRCS
    l2tp.cpp
    l2tpwidget.cpp
    l2tpauth.cpp
    l2tpadvancedwidget.cpp
    l2tppppwidget.cpp
)

kde4_add_ui_files(l2tp_SRCS l2tp.ui l2tpadvanced.ui l2tpauth.ui l2tpppp.ui)

kde4_add_plugin(plasmanm_l2tpui ${l2tp_SRCS})

target_link_libraries(plasmanm_l2tpui plasmanm-internal NetworkManagerQt ${KDE4_KIO_LIBS})

install(TARGETS plasmanm_l2tpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm_l2tpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
