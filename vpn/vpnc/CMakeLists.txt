set(vpnc_SRCS
    vpnc.cpp
    vpncwidget.cpp
    vpncadvancedwidget.cpp
    vpncauth.cpp
)

qt5_wrap_ui(vpnc_SRCS vpnc.ui vpncadvanced.ui vpncauth.ui)

kservice_desktop_to_json(plasmanetworkmanagement_vpncui.desktop)

plasma_add_plugin(plasmanetworkmanagement_vpncui ${vpnc_SRCS})

target_link_libraries(plasmanetworkmanagement_vpncui
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
    KF5::KDE4Support
)

install(TARGETS plasmanetworkmanagement_vpncui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

install(FILES plasmanetworkmanagement_vpncui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
