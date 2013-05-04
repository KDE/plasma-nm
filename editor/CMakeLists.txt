set(connectioneditor_SRCS
   main.cpp
   connectioneditor.cpp
   connectionitem.cpp
   connectiontypeitem.cpp
   connectiondetaileditor.cpp
   connectionwidget.cpp
   wiredconnectionwidget.cpp
   wificonnectionwidget.cpp
   ipv4widget.cpp
   ui/ipv4routeswidget.cpp
   ui/delegate.cpp
   ui/intdelegate.cpp
   ui/ipv4delegate.cpp
   ui/simpleipv4addressvalidator.cpp
   ipv6widget.cpp
   ui/ipv6routeswidget.cpp
   ui/ipv6delegate.cpp
   ui/simpleipv6addressvalidator.cpp
   wifisecurity.cpp
   wiredsecurity.cpp
   security802-1x.cpp
   pppwidget.cpp
   pppoewidget.cpp
   gsmwidget.cpp
   cdmawidget.cpp
   btwidget.cpp
   infinibandwidget.cpp
   bondwidget.cpp
   bridgewidget.cpp
   vlanwidget.cpp
)

kde4_add_ui_files(connectioneditor_SRCS
    ui/connectioneditor.ui
    ui/connectiondetaileditor.ui
    ui/connectionwidget.ui
    ui/wiredconnectionwidget.ui
    ui/wificonnectionwidget.ui
    ui/ipv4.ui
    ui/ipv4routes.ui
    ui/ipv6.ui
    ui/ipv6routes.ui
    ui/wifisecurity.ui
    ui/wiredsecurity.ui
    ui/802-1x.ui
    ui/ppp.ui
    ui/pppoe.ui
    ui/gsm.ui
    ui/cdma.ui
    ui/bt.ui
    ui/infiniband.ui
    ui/bond.ui
    ui/bridge.ui
    ui/vlan.ui
)

kde4_add_executable(kde-nm-connection-editor ${connectioneditor_SRCS})
target_link_libraries(kde-nm-connection-editor plasmanm-internal NetworkManagerQt ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kde-nm-connection-editor DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kde-nm-connection-editor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
