add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_l2tpui\")



add_library(plasmanetworkmanagement_l2tpui MODULE)
target_sources(plasmanetworkmanagement_l2tpui PRIVATE
    l2tp.cpp
    l2tpwidget.cpp
    l2tpauth.cpp
    l2tpipsecwidget.cpp
    l2tppppwidget.cpp
)
ki18n_wrap_ui(plasmanetworkmanagement_l2tpui l2tp.ui l2tpipsec.ui l2tpppp.ui)

kcoreaddons_desktop_to_json(plasmanetworkmanagement_l2tpui plasmanetworkmanagement_l2tpui.desktop)

target_link_libraries(plasmanetworkmanagement_l2tpui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_l2tpui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
