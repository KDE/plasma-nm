include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)

set(plasmanm_editor_SRCS
    settings/bondwidget.cpp
    settings/bridgewidget.cpp
    settings/btwidget.cpp
    settings/cdmawidget.cpp
    settings/connectionwidget.cpp
    settings/gsmwidget.cpp
    settings/infinibandwidget.cpp
    settings/ipv4widget.cpp
    settings/ipv6widget.cpp
    settings/pppoewidget.cpp
    settings/pppwidget.cpp
    settings/security802-1x.cpp
    settings/teamwidget.cpp
    settings/vlanwidget.cpp
    settings/wificonnectionwidget.cpp
    settings/wifisecurity.cpp
    settings/wiredconnectionwidget.cpp
    settings/wiredsecurity.cpp
    settings/wireguardinterfacewidget.cpp
    settings/wireguardtabwidget.cpp
    settings/wireguardpeerwidget.cpp

    widgets/advancedpermissionswidget.cpp
    widgets/bssidcombobox.cpp
    widgets/delegate.cpp
    widgets/editlistdialog.cpp
    widgets/hwaddrcombobox.cpp
    widgets/intdelegate.cpp
    widgets/ipv4delegate.cpp
    widgets/ipv4routeswidget.cpp
    widgets/ipv6delegate.cpp
    widgets/ipv6routeswidget.cpp
    widgets/passwordfield.cpp
    widgets/settingwidget.cpp
    widgets/ssidcombobox.cpp

    connectioneditorbase.cpp
    connectioneditordialog.cpp
    connectioneditortabwidget.cpp
    listvalidator.cpp
    simpleipv4addressvalidator.cpp
    simpleipv6addressvalidator.cpp
    simpleiplistvalidator.cpp
    wireguardkeyvalidator.cpp
    vpnuiplugin.cpp

    ../configuration.cpp
    ../debug.cpp
    ../uiutils.cpp
)

if (WITH_MODEMMANAGER_SUPPORT)
    set(plasmanm_editor_SRCS
        ${plasmanm_editor_SRCS}
        widgets/mobileconnectionwizard.cpp
        mobileproviders.cpp)
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
endif()

install(TARGETS plasmanm_editor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma-networkmanagement-vpnuiplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})
