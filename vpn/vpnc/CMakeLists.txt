add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-vpncui\")

set(vpnc_SRCS
    vpnc.cpp
    vpncwidget.cpp
    vpncadvancedwidget.cpp
    vpncauth.cpp
)

ki18n_wrap_ui(vpnc_SRCS vpnc.ui vpncadvanced.ui vpncauth.ui)

add_library(plasmanetworkmanagement_vpncui ${vpnc_SRCS})

kservice_desktop_to_json(plasmanetworkmanagement_vpncui plasmanetworkmanagement_vpncui.desktop)

target_link_libraries(plasmanetworkmanagement_vpncui
    plasmanm_internal
    plasmanm_editor
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::NetworkManagerQt
    KF5::Service
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
    KF5::KDELibs4Support
)

install(TARGETS plasmanetworkmanagement_vpncui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

install(FILES plasmanetworkmanagement_vpncui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
