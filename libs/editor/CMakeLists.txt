include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)

set(plasmanetworkmanagement_libeditor_SRCS
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
    settings/vlanwidget.cpp
    settings/wificonnectionwidget.cpp
    settings/wifisecurity.cpp
    settings/wimaxwidget.cpp
    settings/wiredconnectionwidget.cpp
    settings/wiredsecurity.cpp

    widgets/advancedpermissionswidget.cpp
    widgets/bssidcombobox.cpp
    widgets/delegate.cpp
    widgets/hwaddrcombobox.cpp
    widgets/intdelegate.cpp
    widgets/ipv4delegate.cpp
    widgets/ipv4routeswidget.cpp
    widgets/ipv6delegate.cpp
    widgets/ipv6routeswidget.cpp
    widgets/settingwidget.cpp
    widgets/ssidcombobox.cpp

    connectiondetaileditor.cpp
    simpleipv4addressvalidator.cpp
    simpleipv6addressvalidator.cpp
    vpnuiplugin.cpp
)

if (WITH_MODEMMANAGER_SUPPORT)
    set(plasmanetworkmanagement_libeditor_SRCS
        ${plasmanetworkmanagement_libeditor_SRCS}
        widgets/mobileconnectionwizard.cpp
        mobileproviders.cpp)
endif()

if (${NETWORKMANAGER_VERSION} VERSION_EQUAL 0.9.9.0 OR ${NETWORKMANAGER_VERSION} VERSION_GREATER 0.9.9.0)
set(plasmanetworkmanagement_libeditor_SRCS
    ${plasmanetworkmanagement_libeditor_SRCS}
    settings/teamwidget.cpp)
endif()

qt5_wrap_ui(plasmanetworkmanagement_libeditor_SRCS
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
    settings/ui/wimax.ui
    settings/ui/wiredconnectionwidget.ui
    settings/ui/wiredsecurity.ui

    widgets/ui/advancedpermissionswidget.ui
    widgets/ui/ipv4routes.ui
    widgets/ui/ipv6routes.ui

    connectiondetaileditor.ui
)

add_library(plasmanetworkmanagement-editor SHARED ${plasmanetworkmanagement_libeditor_SRCS})
target_link_libraries(plasmanetworkmanagement-editor
    plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT5_LDFLAGS}
    ${MODEMMANAGERQT5_LDFLAGS}
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::Service
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOCore KF5::KIOWidgets
    KF5::KIOWidgets
    KF5::CoreAddons
    KF5::Wallet
    KF5::ItemViews
    KF5::XmlGui
    KF5::KDE4Support
)

install(TARGETS plasmanetworkmanagement-editor ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma-networkmanagement-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
