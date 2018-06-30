add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_vpncui\")

set(vpnc_SRCS
    ../../libs/debug.cpp
    vpnc.cpp
    vpncwidget.cpp
    vpncadvancedwidget.cpp
    vpncauth.cpp
)

ki18n_wrap_ui(vpnc_SRCS vpnc.ui vpncadvanced.ui vpncauth.ui)

add_library(plasmanetworkmanagement_vpncui ${vpnc_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_vpncui plasmanetworkmanagement_vpncui.desktop)

target_link_libraries(plasmanetworkmanagement_vpncui
    plasmanm_internal
    plasmanm_editor
    KF5::ConfigCore
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
)

install(TARGETS plasmanetworkmanagement_vpncui  DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES plasmanetworkmanagement_vpncui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
