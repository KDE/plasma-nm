set(plasmanetworkmanagement_libeditor_SRCS
    ipv4widget.cpp
    ui/advancedpermissionswidget.cpp
    ui/delegate.cpp
    ui/ipv4delegate.cpp
    ui/ipv4routeswidget.cpp
    ui/simpleipv4addressvalidator.cpp
    ui/intdelegate.cpp
    ipv6widget.cpp
    ui/ipv6routeswidget.cpp
    ui/ipv6delegate.cpp
    ui/simpleipv6addressvalidator.cpp
    security802-1x.cpp
    wiredconnectionwidget.cpp
    wiredsecurity.cpp
    wificonnectionwidget.cpp
    wifisecurity.cpp
    pppwidget.cpp
    pppoewidget.cpp
    gsmwidget.cpp
    cdmawidget.cpp
    btwidget.cpp
    infinibandwidget.cpp
    vlanwidget.cpp
    wimaxwidget.cpp
    bondwidget.cpp
    bridgewidget.cpp
    connectiondetaileditor.cpp
    connectionwidget.cpp
)

if (${NETWORKMANAGERQT_VERSION} VERSION_GREATER 0.9.8.9)
set(plasmanetworkmanagement_libeditor_SRCS
    ${plasmanetworkmanagement_libeditor_SRCS}
    teamwidget.cpp)
endif()

kde4_add_ui_files(plasmanetworkmanagement_libeditor_SRCS
    ui/advancedpermissionswidget.ui
    ui/ipv4.ui
    ui/ipv4routes.ui
    ui/ipv6.ui
    ui/ipv6routes.ui
    ui/802-1x.ui
    ui/wiredconnectionwidget.ui
    ui/wiredsecurity.ui
    ui/wificonnectionwidget.ui
    ui/wifisecurity.ui
    ui/ppp.ui
    ui/pppoe.ui
    ui/gsm.ui
    ui/cdma.ui
    ui/bt.ui
    ui/infiniband.ui
    ui/vlan.ui
    ui/wimax.ui
    ui/bond.ui
    ui/bridge.ui
    ui/connectiondetaileditor.ui
    ui/connectionwidget.ui
)

if (${NETWORKMANAGERQT_VERSION} VERSION_GREATER 0.9.8.9)
    kde4_add_ui_files(plasmanetworkmanagement_libeditor_SRCS ui/team.ui)
endif()

kde4_add_library(plasmanetworkmanagement-editor SHARED ${plasmanetworkmanagement_libeditor_SRCS})
target_link_libraries(plasmanetworkmanagement-editor
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTXML_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    plasmanetworkmanagement-internal
)

install(TARGETS plasmanetworkmanagement-editor ${INSTALL_TARGETS_DEFAULT_ARGS})
