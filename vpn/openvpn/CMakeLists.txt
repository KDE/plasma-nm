
# Needed to build this module
remove_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

set(openvpn_SRCS
    openvpn.cpp
    openvpnwidget.cpp
    openvpnauth.cpp
    openvpnadvancedwidget.cpp
)

qt5_wrap_ui(openvpn_SRCS openvpn.ui openvpnauth.ui openvpnadvanced.ui)

plasma_add_plugin(plasmanetworkmanagement_openvpnui ${openvpn_SRCS})

kservice_desktop_to_json(plasmanetworkmanagement_openvpnui plasmanetworkmanagement_openvpnui.desktop)

target_link_libraries(plasmanetworkmanagement_openvpnui
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    ${NETWORKMANAGERQT5_LDFLAGS}
    Qt5::DBus
    KF5::KIOCore
    KF5::KIOWidgets
)

install(TARGETS plasmanetworkmanagement_openvpnui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

install(FILES plasmanetworkmanagement_openvpnui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
