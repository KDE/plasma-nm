add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openconnectui\")

macro(add_openconnect_plugin name)
    set(openconnect_SRCS
        ../../libs/debug.cpp
        openconnectui.cpp
        openconnectwidget.cpp
        openconnectauth.cpp
        openconnectauthworkerthread.cpp
        openconnect_${name}.cpp
    )

    ki18n_wrap_ui(openconnect_SRCS openconnectprop.ui openconnectauth.ui openconnecttoken.ui)

    add_library(plasmanetworkmanagement_openconnect_${name} MODULE ${openconnect_SRCS})

    target_link_libraries(plasmanetworkmanagement_openconnect_${name}
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

    kcoreaddons_desktop_to_json(plasmanetworkmanagement_openconnect_${name} "plasmanetworkmanagement_openconnect_${name}.desktop")
    install(TARGETS plasmanetworkmanagement_openconnect_${name}  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
endmacro()

add_openconnect_plugin(anyconnect)
add_openconnect_plugin(juniperui)
add_openconnect_plugin(globalprotectui)
add_openconnect_plugin(pulseui)
