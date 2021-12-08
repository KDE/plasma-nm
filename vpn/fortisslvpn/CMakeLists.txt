add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_fortisslvpnui\")



add_library(plasmanetworkmanagement_fortisslvpnui MODULE)
target_sources(plasmanetworkmanagement_fortisslvpnui PRIVATE
    fortisslvpn.cpp
    fortisslvpnwidget.cpp
    fortisslvpnauth.cpp
)
ki18n_wrap_ui(plasmanetworkmanagement_fortisslvpnui fortisslvpn.ui fortisslvpnadvanced.ui fortisslvpnauth.ui)

kcoreaddons_desktop_to_json(plasmanetworkmanagement_fortisslvpnui plasmanetworkmanagement_fortisslvpnui.desktop)

target_link_libraries(plasmanetworkmanagement_fortisslvpnui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_fortisslvpnui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
