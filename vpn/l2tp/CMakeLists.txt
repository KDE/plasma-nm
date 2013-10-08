set(l2tp_SRCS
    l2tp.cpp
    l2tpwidget.cpp
    l2tpauth.cpp
    l2tpadvancedwidget.cpp
    l2tppppwidget.cpp
)

kde4_add_ui_files(l2tp_SRCS l2tp.ui l2tpadvanced.ui l2tpauth.ui l2tpppp.ui)

kde4_add_plugin(plasmanetworkmanagement_l2tpui ${l2tp_SRCS})

target_link_libraries(plasmanetworkmanagement_l2tpui plasmanetworkmanagement-internal ${NETWORKMANAGERQT_LDFLAGS} ${KDE4_KIO_LIBS})

install(TARGETS plasmanetworkmanagement_l2tpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_l2tpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
