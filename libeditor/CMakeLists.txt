set(plasmanm_libeditor_SRCS
    ipv4widget.cpp
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
)

kde4_add_ui_files(plasmanm_libeditor_SRCS
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
)

kde4_add_library(libeditor SHARED ${plasmanm_libeditor_SRCS})
target_link_libraries(libeditor
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTXML_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    NetworkManagerQt
    ModemManagerQt
    plasmanm-internal
)

install(TARGETS libeditor ${INSTALL_TARGETS_DEFAULT_ARGS})
