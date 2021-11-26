set(plasmanm_editor_SRCS
    connectioneditorbase.cpp
    connectioneditorbase.h
    connectioneditordialog.cpp
    connectioneditordialog.h
    connectioneditortabwidget.cpp
    connectioneditortabwidget.h
    listvalidator.cpp
    listvalidator.h
    settings/bondwidget.cpp
    settings/bondwidget.h
    settings/bridgewidget.cpp
    settings/bridgewidget.h
    settings/btwidget.cpp
    settings/btwidget.h
    settings/cdmawidget.cpp
    settings/cdmawidget.h
    settings/connectionwidget.cpp
    settings/connectionwidget.h
    settings/gsmwidget.cpp
    settings/gsmwidget.h
    settings/infinibandwidget.cpp
    settings/infinibandwidget.h
    settings/ipv4widget.cpp
    settings/ipv4widget.h
    settings/ipv6widget.cpp
    settings/ipv6widget.h
    settings/pppoewidget.cpp
    settings/pppoewidget.h
    settings/pppwidget.cpp
    settings/pppwidget.h
    settings/security802-1x.cpp
    settings/security802-1x.h
    settings/teamwidget.cpp
    settings/teamwidget.h
    settings/vlanwidget.cpp
    settings/vlanwidget.h
    settings/wificonnectionwidget.cpp
    settings/wificonnectionwidget.h
    settings/wifisecurity.cpp
    settings/wifisecurity.h
    settings/wiredconnectionwidget.cpp
    settings/wiredconnectionwidget.h
    settings/wiredsecurity.cpp
    settings/wiredsecurity.h
    settings/wireguardinterfacewidget.cpp
    settings/wireguardinterfacewidget.h
    settings/wireguardpeerwidget.cpp
    settings/wireguardpeerwidget.h
    settings/wireguardtabwidget.cpp
    settings/wireguardtabwidget.h

    simpleiplistvalidator.cpp
    simpleiplistvalidator.h
    simpleipv4addressvalidator.cpp
    simpleipv4addressvalidator.h
    simpleipv6addressvalidator.cpp
    simpleipv6addressvalidator.h
    vpnuiplugin.cpp
    vpnuiplugin.h

    widgets/advancedpermissionswidget.cpp
    widgets/advancedpermissionswidget.h
    widgets/bssidcombobox.cpp
    widgets/bssidcombobox.h
    widgets/delegate.cpp
    widgets/delegate.h
    widgets/editlistdialog.cpp
    widgets/editlistdialog.h
    widgets/hwaddrcombobox.cpp
    widgets/hwaddrcombobox.h
    widgets/intdelegate.cpp
    widgets/intdelegate.h
    widgets/ipv4delegate.cpp
    widgets/ipv4delegate.h
    widgets/ipv4routeswidget.cpp
    widgets/ipv4routeswidget.h
    widgets/ipv6delegate.cpp
    widgets/ipv6delegate.h
    widgets/ipv6routeswidget.cpp
    widgets/ipv6routeswidget.h
    widgets/passwordfield.cpp
    widgets/passwordfield.h
    widgets/settingwidget.cpp
    widgets/settingwidget.h
    widgets/ssidcombobox.cpp
    widgets/ssidcombobox.h
    wireguardkeyvalidator.cpp
    wireguardkeyvalidator.h

    ../configuration.cpp
    ../debug.cpp
    ../uiutils.cpp
)

if (WITH_MODEMMANAGER_SUPPORT)
    set(plasmanm_editor_SRCS
        ${plasmanm_editor_SRCS}
        widgets/mobileconnectionwizard.cpp
        widgets/mobileconnectionwizard.h
        mobileproviders.cpp
        mobileproviders.h)
endif()

ki18n_wrap_ui(plasmanm_editor_SRCS
    settings/ui/802-1x.ui
    settings/ui/bond.ui
    settings/ui/bridge.ui
    settings/ui/bt.ui
    settings/ui/cdma.ui
    settings/ui/connectionwidget.ui
    settings/ui/gsm.ui
    settings/ui/infiniband.ui
    settings/ui/ipv4.ui
    settings/ui/ipv6.ui
    settings/ui/ppp.ui
    settings/ui/pppoe.ui
    settings/ui/team.ui
    settings/ui/vlan.ui
    settings/ui/team.ui
    settings/ui/wificonnectionwidget.ui
    settings/ui/wifisecurity.ui
    settings/ui/wiredconnectionwidget.ui
    settings/ui/wiredsecurity.ui
    settings/ui/wireguardinterfacewidget.ui
    settings/ui/wireguardtabwidget.ui
    settings/ui/wireguardpeerwidget.ui

    widgets/ui/advancedpermissionswidget.ui
    widgets/ui/ipv4routes.ui
    widgets/ui/ipv6routes.ui

    connectioneditortabwidget.ui
)

add_library(plasmanm_editor SHARED ${plasmanm_editor_SRCS})

target_include_directories(plasmanm_editor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    PRIVATE
        ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(plasmanm_editor
PUBLIC
    KF5::ConfigWidgets
    KF5::Completion
    KF5::NetworkManagerQt
    KF5::WidgetsAddons
    Qt::Widgets
PRIVATE
    KF5::I18n
    KF5::KIOWidgets
    KF5::Notifications
    KF5::Solid
    KF5::Wallet
    Qt::DBus
    Qt::Network
    qca-qt5
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_editor PUBLIC KF5::ModemManagerQt)
    target_compile_definitions(plasmanm_editor PRIVATE BROADBANDPROVIDER_DATABASE=\"${BROADBANDPROVIDER_DATABASE}\")
endif()

install(TARGETS plasmanm_editor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
