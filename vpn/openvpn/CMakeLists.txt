add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openvpnui\")

# Needed to build this module
remove_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

set(openvpn_SRCS
    openvpn.cpp
    openvpnwidget.cpp
    openvpnauth.cpp
    openvpnadvancedwidget.cpp
)

ki18n_wrap_ui(openvpn_SRCS openvpn.ui openvpnadvanced.ui)

add_library(plasmanetworkmanagement_openvpnui ${openvpn_SRCS})

kservice_desktop_to_json(plasmanetworkmanagement_openvpnui plasmanetworkmanagement_openvpnui.desktop)

target_link_libraries(plasmanetworkmanagement_openvpnui
    plasmanm_internal
    plasmanm_editor
    Qt5::DBus
    KF5::NetworkManagerQt
    KF5::KIOCore
    KF5::KIOWidgets
)

install(TARGETS plasmanetworkmanagement_openvpnui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_openvpnui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
