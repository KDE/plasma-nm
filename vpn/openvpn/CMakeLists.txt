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

kcoreaddons_desktop_to_json(plasmanetworkmanagement_openvpnui plasmanetworkmanagement_openvpnui.desktop)

target_link_libraries(plasmanetworkmanagement_openvpnui
    plasmanm_internal
    plasmanm_editor
    KF5::KIOWidgets
    KF5::I18n
    KF5::WidgetsAddons
    KF5::CoreAddons
)

install(TARGETS plasmanetworkmanagement_openvpnui  DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES plasmanetworkmanagement_openvpnui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
