include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)

set(plasmanm_editor_SRCS
#     settings/bondwidget.cpp
#     settings/bridgewidget.cpp
    settings/btwidget.cpp
    settings/cdmawidget.cpp
    settings/connectionwidget.cpp
    settings/gsmwidget.cpp
#     settings/infinibandwidget.cpp
    settings/ipv4widget.cpp
    settings/ipv6widget.cpp
    settings/pppoewidget.cpp
    settings/pppwidget.cpp
    settings/security802-1x.cpp
#     settings/vlanwidget.cpp
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

    ../uiutils.cpp
)

if (WITH_MODEMMANAGER_SUPPORT)
    set(plasmanm_editor_SRCS
        ${plasmanm_editor_SRCS}
        widgets/mobileconnectionwizard.cpp
        mobileproviders.cpp)
endif()

# if (${NETWORKMANAGER_VERSION} VERSION_EQUAL 0.9.9.0 OR ${NETWORKMANAGER_VERSION} VERSION_GREATER 0.9.9.0)
# set(plasmanm_editor_SRCS
#     ${plasmanm_editor_SRCS}
#     settings/teamwidget.cpp)
# endif()

ki18n_wrap_ui(plasmanm_editor_SRCS
    settings/ui/802-1x.ui
#     settings/ui/bond.ui
#     settings/ui/bridge.ui
    settings/ui/bt.ui
    settings/ui/cdma.ui
    settings/ui/connectionwidget.ui
    settings/ui/gsm.ui
#     settings/ui/infiniband.ui
    settings/ui/ipv4.ui
    settings/ui/ipv6.ui
    settings/ui/ppp.ui
    settings/ui/pppoe.ui
#     settings/ui/team.ui
#     settings/ui/vlan.ui
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

add_library(plasmanm_editor SHARED ${plasmanm_editor_SRCS})
target_link_libraries(plasmanm_editor
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::NetworkManagerQt
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
    KF5::KDELibs4Support
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_editor KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_editor ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma-networkmanagement-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
