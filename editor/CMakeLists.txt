include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(connectioneditor_SRCS
   main.cpp
   connectioneditor.cpp
   connectionitem.cpp
   connectiontypeitem.cpp
   connectiondetaileditor.cpp
   settingwidget.cpp
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
)

set(connectioneditor_HDRS
   connectioneditor.h
   connectionitem.h
   connectiontypeitem.h
   connectiondetaileditor.h
   settingwidget.h
   connectionwidget.h
   wiredconnectionwidget.h
   wificonnectionwidget.h
   ipv4widget.h
   ui/ipv4routeswidget.h
   ui/delegate.h
   ui/intdelegate.h
   ui/ipv4delegate.h
   ui/simpleipv4addressvalidator.h
   ipv6widget.h
   ui/ipv6routeswidget.h
   ui/ipv6delegate.h
   ui/simpleipv6addressvalidator.h
   wifisecurity.h
   wiredsecurity.h
   security802-1x.h
   pppwidget.h
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
)
qt4_wrap_cpp(connectioneditor_HDRS_MOC ${connectioneditor_HDRS})

add_executable(plasma-nm-connectioneditor ${connectioneditor_SRCS} ${connectioneditor_HDRS_MOC})
target_link_libraries(plasma-nm-connectioneditor QtNetworkManager ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

