add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openconnectui\")

set(openconnect_SRCS
    ../../libs/debug.cpp
    openconnectui.cpp
    openconnectwidget.cpp
    openconnectauth.cpp
    openconnectauthworkerthread.cpp
)

ki18n_wrap_ui(openconnect_SRCS openconnectprop.ui openconnectauth.ui openconnecttoken.ui)

add_library(plasmanetworkmanagement_openconnectui ${openconnect_SRCS})

target_link_libraries(plasmanetworkmanagement_openconnectui
    plasmanm_internal
    plasmanm_editor
    KF5::NetworkManagerQt
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
    Qt::Xml
    PkgConfig::OPENCONNECT
)

install(TARGETS plasmanetworkmanagement_openconnectui  DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES plasmanetworkmanagement_openconnectui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES plasmanetworkmanagement_openconnect_juniperui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES plasmanetworkmanagement_openconnect_globalprotectui.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES plasmanetworkmanagement_openconnect_pulseui.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
