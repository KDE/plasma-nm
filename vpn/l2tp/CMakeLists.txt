set(l2tp_SRCS
    l2tp.cpp
    l2tpwidget.cpp
    l2tpauth.cpp
    l2tpadvancedwidget.cpp
    l2tppppwidget.cpp
)

qt5_wrap_ui(l2tp_SRCS l2tp.ui l2tpadvanced.ui l2tpauth.ui l2tpppp.ui)

plasma_add_plugin(plasmanetworkmanagement_l2tpui ${l2tp_SRCS})

target_link_libraries(plasmanetworkmanagement_l2tpui
    plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT_LDFLAGS}
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::KService
    KF5::KCompletion
    KF5::KI18n
    KF5::KWidgetsAddons
    KF5::KIOWidgets
    KF5::KCoreAddons
    ${KDE4Support_LIBRARIES}
)

install(TARGETS plasmanetworkmanagement_l2tpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_l2tpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
