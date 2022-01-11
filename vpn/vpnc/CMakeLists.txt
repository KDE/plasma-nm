add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_vpncui\")

add_library(plasmanetworkmanagement_vpncui MODULE)
target_sources(plasmanetworkmanagement_vpncui PRIVATE
    vpnc.cpp
    vpncwidget.cpp
    vpncadvancedwidget.cpp
    vpncauth.cpp
)
ecm_qt_declare_logging_category(plasmanetworkmanagement_vpncui HEADER plasma_nm_vpnc.h IDENTIFIER PLASMA_NM_VPNC_LOG CATEGORY_NAME org.kde.plasma.nm.openvpn DESCRIPTION "Plasma NM (vpnc plugins)" EXPORT PLASMANM)

ki18n_wrap_ui(plasmanetworkmanagement_vpncui vpnc.ui vpncadvanced.ui vpncauth.ui)

target_link_libraries(plasmanetworkmanagement_vpncui
    plasmanm_internal
    plasmanm_editor
    KF5::ConfigCore
    KF5::CoreAddons
    KF5::KIOWidgets
    KF5::I18n
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_vpncui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
