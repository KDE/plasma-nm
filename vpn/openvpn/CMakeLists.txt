add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openvpnui\")

# Needed to build this module
remove_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

set(openvpn_SRCS
    openvpn.cpp
    openvpnwidget.cpp
    openvpnauth.cpp
    openvpnadvancedwidget.cpp
)
ecm_qt_declare_logging_category(openvpn_SRCS HEADER plasma_nm_openvpn.h IDENTIFIER PLASMA_NM_OPENVPN_LOG CATEGORY_NAME org.kde.plasma.nm.openvpn DESCRIPTION "Plasma NM (openvpn plugins)" EXPORT PLASMANM)

ki18n_wrap_ui(openvpn_SRCS openvpn.ui openvpnadvanced.ui)

add_library(plasmanetworkmanagement_openvpnui MODULE ${openvpn_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_openvpnui plasmanetworkmanagement_openvpnui.desktop)

target_link_libraries(plasmanetworkmanagement_openvpnui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
)

install(TARGETS plasmanetworkmanagement_openvpnui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
